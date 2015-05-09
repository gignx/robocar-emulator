package justine.robocar;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import org.jxmapviewer.viewer.DefaultWaypoint;

public class Traffic {

    public LinkedList<WaypointPolice> copList = new LinkedList<WaypointPolice>();
    public LinkedList<WaypointGangster> gangsterList = new LinkedList<WaypointGangster>();
    public LinkedList<WaypointCaught> caughtList = new LinkedList<WaypointCaught>();
    public LinkedList<DefaultWaypoint> defaultList = new LinkedList<DefaultWaypoint>();
    public String title;
    public static Map<String, CopTeamData> cop_teams = new HashMap<String, CopTeamData>();
    public String longestTeamName = "";
    public static Map<Integer, Boolean> clicked_map = new HashMap<Integer, Boolean>();

    public Traffic() {
	super();
    }

    public Traffic(LinkedList<WaypointPolice> copList, LinkedList<WaypointGangster> gangsterList, LinkedList<WaypointCaught> caughtList, LinkedList<DefaultWaypoint> defaultList, String title, Map<String, CopTeamData> cop_teams, String longestTeamName) {
	this.copList.addAll(copList);
	this.gangsterList.addAll(gangsterList);
	this.caughtList.addAll(caughtList);
	this.defaultList.addAll(defaultList);
	this.title = title;
	Traffic.cop_teams.putAll(cop_teams);
	this.longestTeamName = longestTeamName;
    }

    public static Traffic create(Traffic copy) {
	Traffic t = new Traffic((LinkedList) copy.copList.clone(), (LinkedList) copy.gangsterList.clone(), (LinkedList) copy.caughtList.clone(), (LinkedList) copy.defaultList.clone(), copy.title, Traffic.cop_teams, copy.longestTeamName);
	return t;
    }

}
