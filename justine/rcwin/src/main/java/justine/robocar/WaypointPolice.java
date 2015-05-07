package justine.robocar;

import org.jxmapviewer.viewer.GeoPosition;
import org.jxmapviewer.viewer.Waypoint;

class WaypointPolice implements Waypoint {

    GeoPosition geoPosition;
    String name;
    int num_caught_, id_;
    long node_from_, node_to_;

    public WaypointPolice(double lat, double lon, String name, int num_caught, long node_from, long node_to, int id) {
	geoPosition = new GeoPosition(lat, lon);
	this.name = name;
	num_caught_ = num_caught;
	node_from_ = node_from;
	node_to_ = node_to;
	id_ = id;
    }

    public GeoPosition getPosition() {
	return geoPosition;
    }

    String getName() {
	return name;
    }

    int getCaught() {
	return num_caught_;
    }

    long getNodeFrom() {
	return node_from_;
    }

    long getNodeTo() {
	return node_to_;
    }

    int getID() {
	return id_;
    }
}