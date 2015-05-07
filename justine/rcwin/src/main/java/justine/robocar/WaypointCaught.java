package justine.robocar;

import org.jxmapviewer.viewer.GeoPosition;
import org.jxmapviewer.viewer.Waypoint;

class WaypointCaught implements Waypoint {

    GeoPosition geoPosition;

    public WaypointCaught(double lat, double lon) {
	geoPosition = new GeoPosition(lat, lon);
    }

    public GeoPosition getPosition() {
	return geoPosition;
    }
}