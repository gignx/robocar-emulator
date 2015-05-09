package justine.robocar;

import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;
import java.util.LinkedList;
import java.util.Map;

import javax.swing.ImageIcon;

import org.jxmapviewer.JXMapViewer;
import org.jxmapviewer.viewer.DefaultWaypoint;
import org.jxmapviewer.viewer.GeoPosition;
import org.jxmapviewer.viewer.Waypoint;
import org.jxmapviewer.viewer.WaypointPainter;

public class CarPainter extends WaypointPainter<Waypoint> {

    private BufferedImage buffer;
    private Graphics2D g;

    public Traffic traffic = new Traffic();

    private Image markerImg;
    private Image markerImgPolice;
    private Image markerImgGangster;
    private Image markerImgCaught;
    private StringBuilder scoreboardStringBuilder = new StringBuilder(100);

    public CarPainter() {
	super();
	ClassLoader classLoader = this.getClass().getClassLoader();
	markerImg = new ImageIcon(classLoader.getResource("logo1.png")).getImage();
	markerImgPolice = new ImageIcon(classLoader.getResource("logo2.png")).getImage();
	markerImgGangster = new ImageIcon(classLoader.getResource("logo3.png")).getImage();
	markerImgCaught = new ImageIcon(classLoader.getResource("logo4.png")).getImage();
    }

    public void update(Traffic traffic) {
	if (this.traffic.longestTeamName != traffic.longestTeamName)
	    sbu = true;
	this.traffic = traffic;
    }

    Point2D a = new Point(0, 0);

    @Override
    protected void doPaint(Graphics2D gr, JXMapViewer map, int w, int h) {
	if (traffic != null) {
	    synchronized (traffic) {
		buffer = new BufferedImage((int) map.getViewportBounds().getWidth(), (int) map.getViewportBounds().getHeight(), BufferedImage.TYPE_INT_ARGB);
		g = buffer.createGraphics();

		a.setLocation(map.getViewportBounds().x, map.getViewportBounds().y);
		GeoPosition pos = map.getTileFactory().pixelToGeo(a, map.getZoom());
		Point2D point = map.getTileFactory().geoToPixel(pos, map.getZoom());
		g.translate(-(int) point.getX(), -(int) point.getY());

		paintDefaults(g, map, traffic.defaultList);
		paintCaughts(g, map, traffic.caughtList);
		paintGangsters(g, map, traffic.gangsterList);
		paintCops(g, map, traffic.copList);

		g.translate((int) point.getX(), (int) point.getY());
		paintScoreBoard(g);

		gr.drawImage(buffer, 0, 0, null);
	    }
	}
	//map.repaint();
    }

    Color scorebg = new Color(0, 0, 0, 150);
    Font arial = new Font("Arial", Font.PLAIN, 14);
    FontMetrics font_metrics;
    private int max_name_width;
    private int font_height;
    private int scoreboard_height;
    private int scoreboard_width;
    private boolean sbu = true;

    private void paintScoreBoard(Graphics2D g) {
	g.setPaint(scorebg);
	g.setFont(arial);
	if (sbu) {
	    font_metrics = g.getFontMetrics();
	    max_name_width = font_metrics.stringWidth(traffic.longestTeamName);
	    font_height = font_metrics.getHeight();
	    scoreboard_height = (font_height + 10) * (Traffic.cop_teams.size() + 1) + 5;
	    scoreboard_width = max_name_width + font_height + font_metrics.charWidth('-') * 12 + 15;
	    sbu = false;
	}
	g.fillRoundRect(10, 10, scoreboard_width, scoreboard_height, 10, 10);
	int draw_y = 15;

	for (Map.Entry<String, CopTeamData> entry : Traffic.cop_teams.entrySet()) {
	    String team_name = entry.getKey();
	    CopTeamData team_data = entry.getValue();
	    g.setPaint(team_data.color);
	    g.fillOval(15, draw_y, font_height, font_height);
	    g.setPaint(Color.WHITE);
	    draw_y += font_height;
	    scoreboardStringBuilder.setLength(0);
	    scoreboardStringBuilder.append(team_name);
	    scoreboardStringBuilder.append(" - ");
	    scoreboardStringBuilder.append(team_data.num_caught);
	    g.drawString(scoreboardStringBuilder.toString(), font_height + 5 + 15, draw_y);
	    draw_y += 10;
	}

	scoreboardStringBuilder.setLength(0);
	scoreboardStringBuilder.append("Gangsters: ");
	scoreboardStringBuilder.append(traffic.caughtList.size());
	scoreboardStringBuilder.append("/");
	scoreboardStringBuilder.append(traffic.gangsterList.size());

	draw_y += font_height;
	g.drawString(scoreboardStringBuilder.toString(), 15, draw_y);
    }

    private void paintGangsters(Graphics2D g, JXMapViewer map, LinkedList<WaypointGangster> points) {
	for (WaypointGangster w : points) {
	    paintGangster(g, map, w);
	}
    }

    private void paintGangster(Graphics2D g, JXMapViewer jXMapV, WaypointGangster w) {
	Point2D point = jXMapV.getTileFactory().geoToPixel(w.getPosition(), jXMapV.getZoom());
	if (!jXMapV.getViewportBounds().contains(point))
	    return;
	g.drawImage(markerImgGangster, (int) point.getX() - markerImgGangster.getWidth(jXMapV), (int) point.getY() - markerImgGangster.getHeight(jXMapV), null);
    }

    private void paintCaughts(Graphics2D g, JXMapViewer map, LinkedList<WaypointCaught> points) {
	for (WaypointCaught w : points) {
	    paintCaught(g, map, w);
	}
    }

    private void paintCaught(Graphics2D g, JXMapViewer jXMapV, WaypointCaught w) {
	Point2D point = jXMapV.getTileFactory().geoToPixel(w.getPosition(), jXMapV.getZoom());
	if (!jXMapV.getViewportBounds().contains(point))
	    return;
	g.drawImage(markerImgCaught, (int) point.getX() - markerImgCaught.getWidth(jXMapV), (int) point.getY() - markerImgCaught.getHeight(jXMapV), null);
    }

    private void paintDefaults(Graphics2D g, JXMapViewer map, LinkedList<DefaultWaypoint> points) {
	for (DefaultWaypoint w : points) {
	    paintDefault(g, map, w);
	}
    }

    private void paintDefault(Graphics2D g, JXMapViewer jXMapV, DefaultWaypoint w) {
	Point2D point = jXMapV.getTileFactory().geoToPixel(w.getPosition(), jXMapV.getZoom());
	if (!jXMapV.getViewportBounds().contains(point))
	    return;
	g.drawImage(markerImg, (int) point.getX() - markerImg.getWidth(jXMapV), (int) point.getY() - markerImg.getHeight(jXMapV), null);
    }

    private void paintCops(Graphics2D g, JXMapViewer map, LinkedList<WaypointPolice> points) {
	for (WaypointPolice w : points) {
	    paintCop(g, map, w);
	}
    }

    Font serif = new Font("Serif", Font.BOLD, 14);

    private void paintCop(Graphics2D g, JXMapViewer jXMapV, WaypointPolice w) {

	Point2D point = jXMapV.getTileFactory().geoToPixel(w.getPosition(), jXMapV.getZoom());
	if (!jXMapV.getViewportBounds().contains(point))
	    return;

	g.drawImage(markerImgPolice, (int) point.getX() - markerImgPolice.getWidth(jXMapV), (int) point.getY() - markerImgPolice.getHeight(jXMapV), null);

	Color border_color = Traffic.cop_teams.get(w.getName()).color;
	g.setFont(serif);
	font_metrics = g.getFontMetrics();
	int nameWidth = font_metrics.stringWidth(w.getName());
	int fontHeight = font_metrics.getHeight();

	g.setColor(scorebg);
	if (Traffic.clicked_map.get(w.getID()) == null || Traffic.clicked_map.get(w.getID()) == false) {
	    Rectangle rect = new Rectangle((int) point.getX(), (int) point.getY(), nameWidth + 4, 20);

	    g.fill(rect);
	    g.setColor(border_color);
	    g.draw(rect);
	    g.setColor(Color.WHITE);

	    g.drawString(w.getName(), (int) point.getX() + 2, (int) point.getY() + 20 - 5);
	} else {
	    int boxWidth = Math.max(162, nameWidth);
	    Rectangle rect = new Rectangle((int) point.getX(), (int) point.getY(), boxWidth + 4, fontHeight * 4 + 10);
	    double center_pos = (boxWidth - nameWidth) / 2.0;

	    g.fill(rect);
	    g.setColor(border_color);
	    g.draw(rect);
	    g.setColor(Color.WHITE);

	    String data[] = { w.getName(), "Caught: " + Integer.toString(w.getCaught()), "From: " + Long.toString(w.getNodeFrom()), "To: " + Long.toString(w.getNodeTo()) };

	    for (int i = 0; i < 4; i++) {
		if (i == 0) {
		    g.drawString(data[i], (int) point.getX() + 2 + (int) center_pos, (int) point.getY() + 15);
		} else {
		    g.drawString(data[i], (int) point.getX() + 2, (int) point.getY() + 15 + i * (fontHeight + 2));
		}
	    }
	    jXMapV.setCenterPosition(w.getPosition());
	}

    }
}