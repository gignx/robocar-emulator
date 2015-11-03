package justine.robocar;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Rectangle;
import java.awt.geom.Point2D;
import java.util.LinkedList;

import javax.swing.ImageIcon;

import org.jxmapviewer.JXMapViewer;
import org.jxmapviewer.viewer.GeoPosition;

class WaypointBus extends UpdateableWaypoint {

	private static Image sprite;
	String name;
	Color c;
	int id_;
	static Color borderbg = new Color(0, 0, 0, 150);
	static Font serif = new Font("Serif", Font.BOLD, 14);
	static FontMetrics font_metrics;
	LinkedList<Loc> path;
	Color pathcolor = new Color(0, 66, 255, 170);
	protected static boolean on = false;

	public WaypointBus(GeoPosition from, GeoPosition to, String name, int id
			){
			//,LinkedList<Loc> path) {
		super(from, to);
		//this.path = new LinkedList<Loc>();
		//this.path.addAll(path);
		this.c = Color.YELLOW;
		this.name = name;
		id_ = id;
	}

	@Override
	public void update() {
		super.update();
		path.set(0, new Loc(getPosition().getLatitude(), getPosition()
				.getLongitude()));
	}

	public void drawPath(Graphics2D g, JXMapViewer map) {
		if ((Traffic.clicked_map.get(getID()) != null)
				&& Traffic.clicked_map.get(getID()) == true) {
			g.setColor(pathcolor);
			g.setStroke(new BasicStroke(5));
			Point2D point = map.getTileFactory().geoToPixel(getPosition(),
					map.getZoom());
			for (Loc l : path) {
				Point2D p = map.getTileFactory().geoToPixel(
						new GeoPosition(l.lat, l.lon), map.getZoom());

				g.drawLine((int) point.getX(), (int) point.getY(),
						(int) p.getX(), (int) p.getY());
				point = p;
			}
			g.setStroke(new BasicStroke(1));
		}
	}

	public void drawWithInfo(Graphics2D g, JXMapViewer map) {

		Point2D point = map.getTileFactory().geoToPixel(getPosition(),
				map.getZoom());
		g.setFont(serif);
		font_metrics = g.getFontMetrics();
		int nameWidth = font_metrics.stringWidth(getName() + " jelzésű busz");
		int fontHeight = font_metrics.getHeight();
		int boxWidth = Math.max(162, nameWidth);
		Rectangle rect = new Rectangle((int) point.getX(), (int) point.getY(),
				boxWidth + 4, fontHeight * 4 + 10);
		double center_pos = (boxWidth - nameWidth) / 2.0;

		if (on)
			g.setColor(borderbg);
		else
			g.setColor(new Color(borderbg.getRed(), borderbg.getGreen(),
					borderbg.getBlue(), 20));
		g.fill(rect);
		if (on)
			g.setColor(c);
		else {
			g.setColor(new Color(c.getRed(), c.getGreen(), c.getBlue(), 20));
		}
		g.draw(rect);
		g.setColor(Color.WHITE);

		String data[] = { getName() + " jelzésű busz" };

		if (!on) {
			g.setColor(new Color(255, 255, 255, 20));
		}

		for (int i = 0; i < 4; i++) {
			if (i == 0) {
				g.drawString(data[i],
						(int) point.getX() + 2 + (int) center_pos,
						(int) point.getY() + 15);
			} else {
				g.drawString(data[i], (int) point.getX() + 2,
						(int) point.getY() + 15 + i * (fontHeight + 2));
			}
		}
		point = map.getTileFactory().geoToPixel(getPosition(), map.getZoom());
		g.drawImage(getSprite(),
				(int) point.getX() - getSprite().getWidth(map),
				(int) point.getY() - getSprite().getHeight(map), null);

		// map.setCenterPosition(getPosition());
	}

	@Override
	public void draw(Graphics2D g, JXMapViewer map) {

		if ((Traffic.clicked_map.get(getID()) != null)
				&& Traffic.clicked_map.get(getID()) == true) {
			drawWithInfo(g, map);
			return;
		}

		Point2D point = map.getTileFactory().geoToPixel(getPosition(),
				map.getZoom());
		if (!map.getViewportBounds().contains(point)) {
			onScreen = false;
			return;
		}
		onScreen = true;

		g.setFont(serif);
		font_metrics = g.getFontMetrics();
		int nameWidth = font_metrics.stringWidth(getName());

		g.setColor(borderbg);
		g.drawImage(getSprite(),
				(int) point.getX() - getSprite().getWidth(map),
				(int) point.getY() - getSprite().getHeight(map), null);
		Rectangle rect = new Rectangle((int) point.getX(), (int) point.getY(),
				nameWidth + 4, 20);

		g.fill(rect);
		g.setColor(c);
		g.draw(rect);
		g.setColor(Color.WHITE);

		g.drawString(getName(), (int) point.getX() + 2,
				(int) point.getY() + 20 - 5);

	}

	public Image getSprite() {
		if (sprite == null) {
			ClassLoader classLoader = this.getClass().getClassLoader();
			sprite = new ImageIcon(classLoader.getResource("logo4.png"))
					.getImage();
		}
		return sprite;
	}

	String getName() {
		return name;
	}

	int getID() {
		return id_;
	}

}
