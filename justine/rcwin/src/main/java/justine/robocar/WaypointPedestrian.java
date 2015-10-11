package justine.robocar;

import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.geom.Point2D;

import javax.swing.ImageIcon;

import org.jxmapviewer.JXMapViewer;
import org.jxmapviewer.viewer.GeoPosition;

public class WaypointPedestrian extends UpdateableWaypoint {

    private static Image sprite;

    public WaypointPedestrian(GeoPosition from, GeoPosition to) {
	      super(from, to, 500);
    }

    public Image getSprite() {
	if (sprite == null) {
	    ClassLoader classLoader = this.getClass().getClassLoader();
	    sprite = new ImageIcon(classLoader.getResource("logo5.png")).getImage();
	}
	return sprite;
    }

    @Override
    public void draw(Graphics2D g, JXMapViewer map) {
	Point2D point = map.getTileFactory().geoToPixel(getPosition(), map.getZoom());
	if (!map.getViewportBounds().contains(point)) {
	    onScreen = false;
	    return;
	}
	onScreen = true;
	g.drawImage(getSprite(), (int) point.getX() - getSprite().getWidth(map), (int) point.getY() - getSprite().getHeight(map), null);
    }

}
