package justine.robocar;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

public class Traffic {

    public LinkedList<WaypointPolice> copList = new LinkedList<WaypointPolice>();
    public LinkedList<WaypointBus> busList = new LinkedList<WaypointBus>();
    public LinkedList<WaypointGangster> gangsterList = new LinkedList<WaypointGangster>();
    public LinkedList<WaypointCaught> caughtList = new LinkedList<WaypointCaught>();
    public LinkedList<WaypointNormal> defaultList = new LinkedList<WaypointNormal>();
    public LinkedList<WaypointPedestrian> pedestrianList = new LinkedList<WaypointPedestrian>();
    public String title;
    public Map<String, CopTeamData> cop_teams = new HashMap<String, CopTeamData>();
    public String longestTeamName = "";
    public static Map<Integer, Boolean> clicked_map = new HashMap<Integer, Boolean>();
    public LinkedList<LinkedList<? extends UpdateableWaypoint>> lists = new LinkedList<LinkedList<? extends UpdateableWaypoint>>();
    long timestamp;
    public long seconds;

 Traffic() {
	super();
    }

    public Traffic(LinkedList<WaypointPolice> copList, LinkedList<WaypointBus> busList, LinkedList<WaypointGangster> gangsterList, LinkedList<WaypointCaught> caughtList, LinkedList<WaypointNormal> defaultList, LinkedList<WaypointPedestrian> pedestrianList, String title, Map<String, CopTeamData> cop_teams, String longestTeamName,long minutes) {
	this.copList.addAll(copList);
  this.busList.addAll(busList);
	this.gangsterList.addAll(gangsterList);
	this.caughtList.addAll(caughtList);
	this.defaultList.addAll(defaultList);
  this.pedestrianList.addAll(pedestrianList);
	this.title = title;
	this.cop_teams.putAll(cop_teams);
	this.longestTeamName = longestTeamName;
	this.seconds=minutes;
	lists.clear();
	lists.add(defaultList);
	lists.add(gangsterList);
	lists.add(caughtList);
  lists.add(busList);
	lists.add(copList);
  lists.add(pedestrianList);
	if (timestamp == 0)
	    timestamp = System.currentTimeMillis();
    }

    public static Traffic create(Traffic copy) {
	Map<String, CopTeamData> new_cop_teams = new HashMap<String, CopTeamData>();
	for (Map.Entry<String, CopTeamData> e : copy.cop_teams.entrySet()) {
	    new_cop_teams.put(e.getKey(), new CopTeamData(e.getValue().num_caught, e.getValue().color));
	}
	LinkedList<WaypointPolice> copList = new LinkedList<WaypointPolice>();
  LinkedList<WaypointBus> busList = new LinkedList<WaypointBus>();
	LinkedList<WaypointGangster> gangsterList = new LinkedList<WaypointGangster>();
	LinkedList<WaypointCaught> caughtList = new LinkedList<WaypointCaught>();
	LinkedList<WaypointNormal> defaultList = new LinkedList<WaypointNormal>();
  LinkedList<WaypointPedestrian> pedestrianList = new LinkedList<WaypointPedestrian>();
	copList.addAll(copy.copList);
  busList.addAll(copy.busList);
	gangsterList.addAll(copy.gangsterList);
	caughtList.addAll(copy.caughtList);
	defaultList.addAll(copy.defaultList);
  pedestrianList.addAll(copy.pedestrianList);
	Traffic t = new Traffic(copList,busList, gangsterList, caughtList, defaultList,pedestrianList, copy.title, new_cop_teams, copy.longestTeamName, copy.seconds);
	t.timestamp = copy.timestamp;
	return t;
    }

}
