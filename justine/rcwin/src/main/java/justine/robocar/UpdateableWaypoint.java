package justine.robocar;

import java.awt.Graphics2D;

import org.jxmapviewer.JXMapViewer;
import org.jxmapviewer.viewer.GeoPosition;
import org.jxmapviewer.viewer.Waypoint;

public class UpdateableWaypoint implements Waypoint {

    private GeoPosition current;
    private double steplong, steplat;
    static int max = 20;
    private int step = 0;
    protected boolean onScreen = true;

    public boolean isOnScreen() {
	return onScreen;
    }

    public void draw(Graphics2D g, JXMapViewer map) {
    }

    public UpdateableWaypoint(GeoPosition from, GeoPosition to) {
	if (from.getLatitude() == to.getLatitude() && from.getLongitude() == to.getLongitude())
	    step = max;
	current = from;
	steplong = (to.getLongitude() - from.getLongitude()) / (max);
	steplat = (to.getLatitude() - from.getLatitude()) / (max);
    }

    public void update() {
	if (step < max) {
	    current = new GeoPosition(current.getLatitude() + steplat, current.getLongitude() + steplong);
	    step++;
	}
    }

    public GeoPosition getPosition() {
	return current;

    }

}
