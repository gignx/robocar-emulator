package justine.robocar;

import java.awt.Color;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Scanner;

import javax.swing.SwingUtilities;

import org.jxmapviewer.viewer.DefaultWaypoint;

public class BackgroundThread implements Runnable {

    private Map<Long, Loc> lmap;
    private String hostname;
    private int port;
    private Map<String, CopTeamData> cop_teams = new HashMap<String, CopTeamData>();
    private Color[] available_colors = { Color.BLUE, Color.RED, Color.GREEN, Color.YELLOW, Color.ORANGE, Color.CYAN, Color.MAGENTA, Color.PINK };
    private String longestTeamName = "";
    private NotifyListener notifyListener = null;

    public interface NotifyListener {
	public void onDataChanged(Traffic current);

	public void onException(Exception e);
    }

    public BackgroundThread(Map<Long, Loc> lmap, String hostname, int port, NotifyListener l) {
	super();
	this.lmap = lmap;
	this.hostname = hostname;
	this.port = port;
	notifyListener = l;
    }

    public void run() {
	Socket trafficServer = null;
	Scanner scanner = null;
	try {
	    trafficServer = new Socket(hostname, port);
	    OutputStream os = trafficServer.getOutputStream();
	    DataOutputStream dos = new DataOutputStream(os);

	    dos.writeUTF("<disp>");
	    InputStream is = trafficServer.getInputStream();

	    scanner = new Scanner(is);

	    LinkedList<WaypointPolice> cop_list = new LinkedList<WaypointPolice>();
	    LinkedList<WaypointGangster> gangster_list = new LinkedList<WaypointGangster>();
	    LinkedList<WaypointCaught> caught_list = new LinkedList<WaypointCaught>();
	    LinkedList<DefaultWaypoint> default_list = new LinkedList<DefaultWaypoint>();

	    final Traffic traffic = new Traffic();

	    while(!Thread.interrupted()) {
		cop_list.clear();
		gangster_list.clear();
		caught_list.clear();
		default_list.clear();
		
		int team_counter = 0;
		int time = 0, size = 0, minutes = 0;

		time = scanner.nextInt();
		minutes = scanner.nextInt();
		size = scanner.nextInt();

		long ref_from = 0, ref_to = 0;
		int step = 0, maxstep = 100, type = 0;
		int id = 0;
		double lat, lon;
		double lat2, lon2;
		int num_captured_gangsters = 0;
		String name = "Cop";

		for (CopTeamData value : cop_teams.values()) {
		    value.num_caught = 0;
		}

		for (int i = 0; i < size; ++i) {
		    ref_from = scanner.nextLong();
		    ref_to = scanner.nextLong();
		    maxstep = scanner.nextInt();
		    step = scanner.nextInt();
		    type = scanner.nextInt();
		    if (type == 1) {
			num_captured_gangsters = scanner.nextInt();
			name = scanner.next();
			id = scanner.nextInt();

			if (cop_teams.containsKey(name)) {
			    if (num_captured_gangsters > 0) {
				CopTeamData data = cop_teams.get(name);
				data.num_caught += num_captured_gangsters;
			    }
			} else {
			    cop_teams.put(name, new CopTeamData(num_captured_gangsters, available_colors[team_counter % available_colors.length]));

			    if (name.length() > longestTeamName.length())
				longestTeamName = name;

			    team_counter++;
			}
		    }
		    lat = lmap.get(ref_from).lat;
		    lon = lmap.get(ref_from).lon;

		    lat2 = lmap.get(ref_to).lat;
		    lon2 = lmap.get(ref_to).lon;

		    if (maxstep == 0) {
			maxstep = 1;
		    }

		    lat += step * ((lat2 - lat) / maxstep);
		    lon += step * ((lon2 - lon) / maxstep);

		    switch (type) {
		    case 1:
			cop_list.add(new WaypointPolice(lat, lon, name, num_captured_gangsters, ref_from, ref_to, id));
			break;
		    case 2:
			gangster_list.add(new WaypointGangster(lat, lon));
			break;
		    case 3:
			caught_list.add(new WaypointCaught(lat, lon));
			break;
		    default:
			default_list.add(new DefaultWaypoint(lat, lon));
			break;
		    }

		}
		if (time >= minutes * 60 * 1000 / 200) {
		    // scanner = null;
		}

		StringBuilder sb = new StringBuilder();

		int sec = time / 5;
		int min = sec / 60;
		sec = sec - min * 60;
		time = time - min * 60 * 5 - sec * 5;

		sb.append("|");
		sb.append(min);
		sb.append(":");
		sb.append(sec);
		sb.append(":");
		sb.append(2 * time);
		sb.append("|");
		traffic.copList = cop_list;
		traffic.gangsterList = gangster_list;
		traffic.caughtList = caught_list;
		traffic.defaultList = default_list;
		traffic.title = sb.toString();
		Traffic.cop_teams = cop_teams;
		traffic.longestTeamName = longestTeamName;

		SwingUtilities.invokeAndWait(new Runnable() {

		    public void run() {
			if (notifyListener != null) {
			    notifyListener.onDataChanged(Traffic.create(traffic));
			}
		    }
		});
	    }

	} catch (Exception e) {
	    if (notifyListener != null)
		notifyListener.onException(e);
	} finally {
	    if (trafficServer != null)
		try {
		    trafficServer.close();
		} catch (IOException e) {
		    if (notifyListener != null)
			notifyListener.onException(e);
		}
	    if (scanner != null)
		scanner.close();
	}

    }
}
