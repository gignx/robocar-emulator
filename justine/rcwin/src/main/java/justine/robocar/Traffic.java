package justine.robocar;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import org.jxmapviewer.viewer.DefaultWaypoint;

public class Traffic {

    private LinkedList<WaypointPolice> copList = new LinkedList<WaypointPolice>();
    private LinkedList<WaypointGangster> gangsterList = new LinkedList<WaypointGangster>();
    private LinkedList<WaypointCaught> caughtList = new LinkedList<WaypointCaught>();
    private LinkedList<DefaultWaypoint> defaultList = new LinkedList<DefaultWaypoint>();
    private String title;
    private int num_gangsters;
    private Map<String, CopTeamData> cop_teams = new HashMap<String, CopTeamData>();
    private String longestTeamName = "";
    private Map<Integer, Boolean> clicked_map = new HashMap<Integer, Boolean>();

    public Traffic(LinkedList<WaypointPolice> copList, LinkedList<WaypointGangster> gangsterList, LinkedList<WaypointCaught> caughtList, LinkedList<DefaultWaypoint> defaultList, String title, int num_gangsters, Map<String, CopTeamData> cop_teams, String longestTeamName, Map<Integer, Boolean> clicked_map) {
	this.copList.clear();
	this.copList.addAll(copList);
	this.gangsterList.clear();
	this.gangsterList.addAll(gangsterList);
	this.caughtList.clear();
	this.caughtList.addAll(caughtList);
	this.defaultList.clear();
	this.defaultList.addAll(defaultList);
	this.title = title;
	this.num_gangsters = num_gangsters;
	this.cop_teams.clear();
	this.cop_teams.putAll(cop_teams);
	this.longestTeamName = longestTeamName;
	this.clicked_map.clear();
	this.clicked_map.putAll(clicked_map);
    }

    public LinkedList<WaypointPolice> getCopList() {
	return copList;
    }

    public LinkedList<WaypointGangster> getGangsterList() {
	return gangsterList;
    }

    public LinkedList<WaypointCaught> getCaughtList() {
	return caughtList;
    }

    public LinkedList<DefaultWaypoint> getDefaultList() {
	return defaultList;
    }

    public String getTitle() {
	return title;
    }

    public int getNumGangsters() {
	return num_gangsters;
    }

    public Map<String, CopTeamData> getCopTeams() {
	return cop_teams;
    }

    public String getLongestTeamName() {
	return longestTeamName;
    }

    public Map<Integer, Boolean> getClickedMap() {
	return clicked_map;
    }

}
