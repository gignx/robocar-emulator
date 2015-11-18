package justine.robocar;

import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;

import org.jxmapviewer.JXMapViewer;
import org.jxmapviewer.viewer.GeoPosition;
import org.jxmapviewer.viewer.Waypoint;
import org.jxmapviewer.viewer.WaypointPainter;

public class CarPainter extends WaypointPainter<Waypoint> {

	private BufferedImage buffer;
	private Graphics2D g;

	Traffic traffic;
	ScoreBoard scores = new ScoreBoard();

	public void update(Traffic traffic) {
		this.traffic = traffic;
		scores.update(traffic);
	}

	Point2D a = new Point(0, 0);

	@Override
	protected void doPaint(Graphics2D gr, JXMapViewer map, int w, int h) {
		buffer = new BufferedImage((int) map.getViewportBounds().getWidth(),
				(int) map.getViewportBounds().getHeight(),
				BufferedImage.TYPE_INT_ARGB);
		g = buffer.createGraphics();
		a.setLocation(map.getViewportBounds().x, map.getViewportBounds().y);
		GeoPosition pos = map.getTileFactory().pixelToGeo(a, map.getZoom());
		Point2D point = map.getTileFactory().geoToPixel(pos, map.getZoom());
		g.translate(-(int) point.getX(), -(int) point.getY());
		if (traffic != null) {
			for (WaypointBus way : traffic.busList)
				way.drawPath(g, map);
			for (WaypointPolice way : traffic.copList)
				way.drawPath(g, map);
			for (WaypointPedestrian way : traffic.pedestrianList)
					way.draw(g, map);
			for (WaypointBusStop way : traffic.busstopList)
							way.draw(g, map);
			for (WaypointNormal way : traffic.defaultList)
				way.draw(g, map);
			for (WaypointCaught way : traffic.caughtList)
				way.draw(g, map);
			for (WaypointGangster way : traffic.gangsterList)
				way.draw(g, map);
			for (WaypointPolice way : traffic.copList)
				way.draw(g, map);
			for (WaypointBus way : traffic.busList)
				way.draw(g, map);

			g.translate((int) point.getX(), (int) point.getY());
			scores.draw(g);

		}
		gr.drawImage(buffer, 0, 0, null);
	}

}
