package justine.robocar;

import org.jxmapviewer.viewer.GeoPosition;
import org.jxmapviewer.viewer.Waypoint;

class WaypointGangster implements Waypoint {

    GeoPosition geoPosition;

    public WaypointGangster(double lat, double lon) {
	geoPosition = new GeoPosition(lat, lon);
    }

    public GeoPosition getPosition() {
	return geoPosition;
    }
}