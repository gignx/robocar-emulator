package justine.robocar;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

public class Traffic {

    public LinkedList<WaypointPolice> copList = new LinkedList<WaypointPolice>();
    public LinkedList<WaypointGangster> gangsterList = new LinkedList<WaypointGangster>();
    public LinkedList<WaypointCaught> caughtList = new LinkedList<WaypointCaught>();
    public LinkedList<WaypointNormal> defaultList = new LinkedList<WaypointNormal>();
    public String title;
    public Map<String, CopTeamData> cop_teams = new HashMap<String, CopTeamData>();
    public String longestTeamName = "";
    public static Map<Integer, Boolean> clicked_map = new HashMap<Integer, Boolean>();
    public LinkedList<LinkedList<? extends UpdateableWaypoint>> lists = new LinkedList<LinkedList<? extends UpdateableWaypoint>>();
    long timestamp;

    public Traffic() {
	super();
    }

    public Traffic(LinkedList<WaypointPolice> copList, LinkedList<WaypointGangster> gangsterList, LinkedList<WaypointCaught> caughtList, LinkedList<WaypointNormal> defaultList, String title, Map<String, CopTeamData> cop_teams, String longestTeamName) {
	this.copList.addAll(copList);
	this.gangsterList.addAll(gangsterList);
	this.caughtList.addAll(caughtList);
	this.defaultList.addAll(defaultList);
	this.title = title;
	this.cop_teams.putAll(cop_teams);
	this.longestTeamName = longestTeamName;
	lists.clear();
	lists.add(defaultList);
	lists.add(gangsterList);
	lists.add(caughtList);
	lists.add(copList);
	if (timestamp == 0)
	    timestamp = System.currentTimeMillis();
    }

    public static Traffic create(Traffic copy) {
	Map<String, CopTeamData> new_cop_teams = new HashMap<String, CopTeamData>();
	for (Map.Entry<String, CopTeamData> e : copy.cop_teams.entrySet()) {
	    new_cop_teams.put(e.getKey(), new CopTeamData(e.getValue().num_caught, e.getValue().color));
	}
	LinkedList<WaypointPolice> copList = new LinkedList<WaypointPolice>();
	LinkedList<WaypointGangster> gangsterList = new LinkedList<WaypointGangster>();
	LinkedList<WaypointCaught> caughtList = new LinkedList<WaypointCaught>();
	LinkedList<WaypointNormal> defaultList = new LinkedList<WaypointNormal>();
	copList.addAll(copy.copList);
	gangsterList.addAll(copy.gangsterList);
	caughtList.addAll(copy.caughtList);
	defaultList.addAll(copy.defaultList);
	Traffic t = new Traffic(copList, gangsterList, caughtList, defaultList, copy.title, new_cop_teams, copy.longestTeamName);
	t.timestamp = copy.timestamp;
	return t;
    }

}
