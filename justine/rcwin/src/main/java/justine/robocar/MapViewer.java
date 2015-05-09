package justine.robocar;

import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.geom.Point2D;
import java.util.LinkedList;
import java.util.Map;

import javax.swing.event.MouseInputListener;

import org.jxmapviewer.JXMapViewer;
import org.jxmapviewer.OSMTileFactoryInfo;
import org.jxmapviewer.VirtualEarthTileFactoryInfo;
import org.jxmapviewer.input.CenterMapListener;
import org.jxmapviewer.input.PanKeyListener;
import org.jxmapviewer.input.PanMouseInputListener;
import org.jxmapviewer.input.ZoomMouseWheelListenerCursor;
import org.jxmapviewer.viewer.DefaultTileFactory;
import org.jxmapviewer.viewer.GeoPosition;
import org.jxmapviewer.viewer.TileFactory;

@SuppressWarnings("serial")
public class MapViewer extends JXMapViewer implements KeyListener {

    final TileFactory tileFactoryArray[] = { new DefaultTileFactory(new OSMTileFactoryInfo()), new DefaultTileFactory(new VirtualEarthTileFactoryInfo(VirtualEarthTileFactoryInfo.MAP)), new DefaultTileFactory(new VirtualEarthTileFactoryInfo(VirtualEarthTileFactoryInfo.SATELLITE)), new DefaultTileFactory(new VirtualEarthTileFactoryInfo(VirtualEarthTileFactoryInfo.HYBRID)) };

    public MapViewer(double lat, double lon, final CarPainter carPainter) {
	super();
	setDoubleBuffered(false);
	GeoPosition debrecen = new GeoPosition(lat, lon);
	MouseInputListener mouseListener = new PanMouseInputListener(this);
	this.addKeyListener(this);
	this.addMouseListener(mouseListener);
	this.addMouseMotionListener(mouseListener);
	this.addMouseListener(new CenterMapListener(this));
	this.addMouseWheelListener(new ZoomMouseWheelListenerCursor(this));
	this.addKeyListener(new PanKeyListener(this));
	this.setTileFactory(tileFactoryArray[0]);
	this.setOverlayPainter(carPainter);
	this.setZoom(9);
	this.setAddressLocation(debrecen);
	this.setCenterPosition(debrecen);
	this.addMouseListener(new MouseAdapter() {
	    @Override
	    public void mouseClicked(MouseEvent me) {
		Point2D gp_pt = null;

		LinkedList<WaypointPolice> waypoints_ = carPainter.traffic.copList;
		Map<Integer, Boolean> clicked_map = Traffic.clicked_map;
		for (WaypointPolice waypoint : waypoints_) {
		    gp_pt = getTileFactory().geoToPixel(waypoint.getPosition(), getZoom());

		    Rectangle rect = getViewportBounds();
		    Point converted_gp_pt = new Point((int) gp_pt.getX() - rect.x, (int) gp_pt.getY() - rect.y);

		    if (converted_gp_pt.distance(me.getPoint()) < 22) {
			clicked_map.put(waypoint.getID(), true);
		    } else {
			clicked_map.put(waypoint.getID(), false);
		    }
		    Traffic.clicked_map = clicked_map;

		}

	    }
	});
    }

    int index = 0;

    public void keyPressed(KeyEvent evt) {

	if (evt.getKeyCode() == KeyEvent.VK_CONTROL) {
	    setZoom(getZoom() + 1);
	}

	if (evt.getKeyCode() == KeyEvent.VK_SPACE) {
	    setTileFactory(tileFactoryArray[++index % 4]);
	    repaint();
	}
    }

    public void keyReleased(KeyEvent e) {
    }

    public void keyTyped(KeyEvent e) {
    }

}
