package justine.robocar;

import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;
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
import org.jxmapviewer.viewer.TileFactory;

@SuppressWarnings("serial")
public class MapViewer extends JXMapViewer implements KeyListener {

	final TileFactory tileFactoryArray[] = {
			new DefaultTileFactory(new OSMTileFactoryInfo()),
			new DefaultTileFactory(new VirtualEarthTileFactoryInfo(
					VirtualEarthTileFactoryInfo.MAP)),
			new DefaultTileFactory(new VirtualEarthTileFactoryInfo(
					VirtualEarthTileFactoryInfo.SATELLITE)),
			new DefaultTileFactory(new VirtualEarthTileFactoryInfo(
					VirtualEarthTileFactoryInfo.HYBRID)) };

	public MapViewer(final CarPainter carPainter) {
		super();
		setDoubleBuffered(false);
		MouseInputListener mouseListener = new PanMouseInputListener(this);
		this.addKeyListener(this);
		this.addMouseListener(mouseListener);
		this.addMouseMotionListener(mouseListener);
		this.addMouseListener(new CenterMapListener(this));
		this.addMouseWheelListener(new ZoomMouseWheelListenerCursor(this));
		this.addKeyListener(new PanKeyListener(this));
		this.setTileFactory(tileFactoryArray[0]);
		this.setOverlayPainter(carPainter);
		this.setZoom(17);
		this.addMouseMotionListener(new MouseMotionListener() {

			public void mouseMoved(MouseEvent e) {
				if (carPainter.traffic != null) {
					LinkedList<WaypointPolice> waypoints_ = carPainter.traffic.copList;
					LinkedList<WaypointBus> busses = carPainter.traffic.busList;
				//TEST
				//LinkedList<WaypointBus> waypoints_b = carPainter.traffic.busList;

					for (WaypointPolice waypoint : waypoints_) {
						if (Traffic.clicked_map.get(waypoint.getID()) == null)
							return;
						if (Traffic.clicked_map.get(waypoint.getID())) {
							Point2D point = getTileFactory().geoToPixel(
									waypoint.getPosition(), getZoom());
							Rectangle rect = getViewportBounds();
							Point con_p = new Point(
									(int) point.getX() - rect.x, (int) point
											.getY() - rect.y);
							if (con_p.distance(e.getPoint()) < 22) {
								WaypointPolice.on = true;
								WaypointBus.on = false;
							} else {
								WaypointPolice.on = false;
							}
						}
					}
					for (WaypointBus waypoint : busses) {
						if (Traffic.clicked_map.get(waypoint.getID()) == null)
							return;
						if (Traffic.clicked_map.get(waypoint.getID())) {
							Point2D point = getTileFactory().geoToPixel(
									waypoint.getPosition(), getZoom());
							Rectangle rect = getViewportBounds();
							Point con_p = new Point(
									(int) point.getX() - rect.x, (int) point
											.getY() - rect.y);
							if (con_p.distance(e.getPoint()) < 22) {
								WaypointBus.on = true;
							} else {
								WaypointBus.on = false;
							}
						}
					}
				}
			}

			public void mouseDragged(MouseEvent e) {

			}
		});
		this.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent me) {
				Point2D gp_pt = null;
				if (carPainter.traffic != null) {
					LinkedList<WaypointPolice> waypoints_ = carPainter.traffic.copList;
				LinkedList<WaypointBus> waypoints_b = carPainter.traffic.busList;
			//	LinkedList<WaypointBusStop> waypoints_bt = carPainter.traffic.busstopList;
				//TESTEND
					Map<Integer, Boolean> clicked_map = Traffic.clicked_map;
					for (WaypointPolice waypoint : waypoints_) {
						gp_pt = getTileFactory().geoToPixel(
								waypoint.getPosition(), getZoom());
					}
					for (WaypointPolice waypoint : waypoints_) {
						gp_pt = getTileFactory().geoToPixel(
								waypoint.getPosition(), getZoom());

						Rectangle rect = getViewportBounds();
						Point converted_gp_pt = new Point((int) gp_pt.getX()
								- rect.x, (int) gp_pt.getY() - rect.y);

						if (converted_gp_pt.distance(me.getPoint()) < 22) {
							clicked_map.put(waypoint.getID(), true);
						} else {
							clicked_map.put(waypoint.getID(), false);
						}
						//Traffic.clicked_map = clicked_map;
					}
					//TEST
					for (WaypointBus waypoint : waypoints_b) {
						gp_pt = getTileFactory().geoToPixel(
								waypoint.getPosition(), getZoom());


						Rectangle rect = getViewportBounds();
						Point converted_gp_pt = new Point((int) gp_pt.getX()
								- rect.x, (int) gp_pt.getY() - rect.y);

						if (converted_gp_pt.distance(me.getPoint()) < 22) {
							clicked_map.put(waypoint.getID(), true);
							for(WaypointBusStop stop : carPainter.traffic.busstopList){
								if(waypoint.stops.contains(stop.getID()))
									Traffic.bus_stop_on_map.put(stop.getID(), true);
								else
									Traffic.bus_stop_on_map.put(stop.getID(), false);
							}
						} else {
							clicked_map.put(waypoint.getID(), false);
						}
						}
					Traffic.clicked_map = clicked_map;
					}
					//TEST END
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
