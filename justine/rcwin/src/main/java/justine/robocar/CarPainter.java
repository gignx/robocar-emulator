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
import java.util.HashMap;
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
    private boolean first = true;
    private Graphics2D g;
    private LinkedList<WaypointPolice> copList = new LinkedList<WaypointPolice>();
    private LinkedList<WaypointGangster> gangsterList = new LinkedList<WaypointGangster>();
    private LinkedList<WaypointCaught> caughtList = new LinkedList<WaypointCaught>();
    private LinkedList<DefaultWaypoint> defaultList = new LinkedList<DefaultWaypoint>();
    private Image markerImg;
    private Image markerImgPolice;
    private Image markerImgGangster;
    private Image markerImgCaught;
    private String longestTeamName = "";
    private int num_gangsters = 0;
    private Map<Integer, Boolean> clicked_map = new HashMap<Integer, Boolean>();
    private Map<String, CopTeamData> cop_teams = new HashMap<String, CopTeamData>();
    private StringBuilder scoreboardStringBuilder = new StringBuilder(100);

    public CarPainter() {
	super();
	ClassLoader classLoader = this.getClass().getClassLoader();
	markerImg = new ImageIcon(classLoader.getResource("logo1.png")).getImage();
	markerImgPolice = new ImageIcon(classLoader.getResource("logo2.png")).getImage();
	markerImgGangster = new ImageIcon(classLoader.getResource("logo3.png")).getImage();
	markerImgCaught = new ImageIcon(classLoader.getResource("logo4.png")).getImage();
    }

    @Override
    protected void doPaint(Graphics2D gr, JXMapViewer map, int w, int h) {
	if (first) {
	    buffer = new BufferedImage((int) map.getViewportBounds().getWidth(), (int) map.getViewportBounds().getHeight(), BufferedImage.TYPE_INT_ARGB);
	    g = buffer.createGraphics();
	}
	Point2D a = new Point(map.getViewportBounds().x, map.getViewportBounds().y);
	GeoPosition pos = map.getTileFactory().pixelToGeo(a, map.getZoom());
	Point2D point = map.getTileFactory().geoToPixel(pos, map.getZoom());
	g.translate(-(int) point.getX(), -(int) point.getY());

	paintCar(g, map, defaultList);
	paintCar(g, map, caughtList);
	paintCar(g, map, gangsterList);
	paintCar(g, map, copList);

	g.translate((int) point.getX(), (int) point.getY());
	paintScoreBoard(g);

	gr.drawImage(buffer, 0, 0, null);
    }

    private void paintCar(Graphics2D g, JXMapViewer map, LinkedList<? extends Waypoint> points) {
	if (points == null)
	    return;
	for (Waypoint w : points) {
	    paintWaypoint(g, map, w);
	}
    }

    private void paintScoreBoard(Graphics2D g) {
	int num_caught = 0;
	g.setPaint(new Color(0, 0, 0, 150));
	g.setFont(new Font("Arial", Font.PLAIN, 14));
	FontMetrics font_metrics = g.getFontMetrics();
	int max_name_width = font_metrics.stringWidth(getLongestTeamName());
	int font_height = font_metrics.getHeight();
	int scoreboard_height = (font_height + 10) * (cop_teams.size() + 1) + 5;
	int scoreboard_width = max_name_width + font_height + font_metrics.charWidth('-') * 12 + 15;
	g.fillRoundRect(10, 10, scoreboard_width, scoreboard_height, 10, 10);
	int draw_y = 15;

	for (Map.Entry<String, CopTeamData> entry : cop_teams.entrySet()) {
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
	    num_caught += team_data.num_caught;
	}

	scoreboardStringBuilder.setLength(0);
	scoreboardStringBuilder.append("Gangsters: ");
	scoreboardStringBuilder.append(num_caught);
	scoreboardStringBuilder.append("/");
	scoreboardStringBuilder.append(getNumGangsters());

	draw_y += font_height;
	g.drawString(scoreboardStringBuilder.toString(), 15, draw_y);
    }

    private void paintWaypoint(Graphics2D g, JXMapViewer jXMapV, Waypoint w) {

	Point2D point = jXMapV.getTileFactory().geoToPixel(w.getPosition(), jXMapV.getZoom());

	if (w instanceof WaypointGangster) {
	    g.drawImage(markerImgGangster, (int) point.getX() - markerImgGangster.getWidth(jXMapV), (int) point.getY() - markerImgGangster.getHeight(jXMapV), null);
	} else if (w instanceof WaypointCaught) {
	    g.drawImage(markerImgCaught, (int) point.getX() - markerImgCaught.getWidth(jXMapV), (int) point.getY() - markerImgCaught.getHeight(jXMapV), null);
	} else if (!(w instanceof WaypointPolice)) {
	    g.drawImage(markerImg, (int) point.getX() - markerImg.getWidth(jXMapV), (int) point.getY() - markerImg.getHeight(jXMapV), null);
	}

	else if (w instanceof WaypointPolice) {
	    g.drawImage(markerImgPolice, (int) point.getX() - markerImgPolice.getWidth(jXMapV), (int) point.getY() - markerImgPolice.getHeight(jXMapV), null);

	    WaypointPolice police = (WaypointPolice) w;

	    Color border_color = getCopTeams().get(police.getName()).color;

	    g.setFont(new Font("Serif", Font.BOLD, 14));
	    FontMetrics fm = g.getFontMetrics();

	    int nameWidth = fm.stringWidth(police.getName());

	    int fontHeight = fm.getHeight();

	    g.setColor(new Color(0, 0, 0, 150));
	    if (clicked_map.get(police.getID()) == null) {
		Rectangle rect = new Rectangle((int) point.getX(), (int) point.getY(), nameWidth + 4, 20);

		g.fill(rect);
		g.setColor(border_color);
		g.draw(rect);
		g.setColor(Color.WHITE);

		g.drawString(police.getName(), (int) point.getX() + 2, (int) point.getY() + 20 - 5);
	    } else if (!clicked_map.get(police.getID())) {
		Rectangle rect = new Rectangle((int) point.getX(), (int) point.getY(), nameWidth + 4, 20);

		g.fill(rect);
		g.setColor(border_color);
		g.draw(rect);
		g.setColor(Color.WHITE);

		g.drawString(police.getName(), (int) point.getX() + 2, (int) point.getY() + 20 - 5);
	    } else {
		int boxWidth = Math.max(162, nameWidth);

		Rectangle rect = new Rectangle((int) point.getX(), (int) point.getY(), boxWidth + 4, fontHeight * 4 + 10);

		double center_pos = (boxWidth - nameWidth) / 2.0;

		g.fill(rect);
		g.setColor(border_color);
		g.draw(rect);
		g.setColor(Color.WHITE);

		String data[] = { police.getName(), "Caught: " + Integer.toString(police.getCaught()), "From: " + Long.toString(police.getNodeFrom()), "To: " + Long.toString(police.getNodeTo()) };

		for (int i = 0; i < 4; i++) {
		    if (i == 0) // team name
		    {
			g.drawString(data[i], (int) point.getX() + 2 + (int) center_pos, (int) point.getY() + 15);
		    } else {
			g.drawString(data[i], (int) point.getX() + 2, (int) point.getY() + 15 + i * (fontHeight + 2));
		    }
		}
		jXMapV.setCenterPosition(police.getPosition());
	    }
	}
    }

    public Map<String, CopTeamData> getCopTeams() {
	return cop_teams;
    }

    public void setCopTeams(Map<String, CopTeamData> cop_teams) {
	this.cop_teams = cop_teams;
    }

    public int getNumGangsters() {
	return num_gangsters;
    }

    public void setNumGangsters(int num_gangsters) {
	this.num_gangsters = num_gangsters;
    }

    public String getLongestTeamName() {
	return longestTeamName;
    }

    public void setLongestTeamName(String longestTeamName) {
	this.longestTeamName = longestTeamName;
    }

    public Map<Integer, Boolean> getClickedMap() {
	return clicked_map;
    }

    public void setClickedMap(Map<Integer, Boolean> clicked_map) {
	this.clicked_map = clicked_map;
    }

    public LinkedList<WaypointPolice> getCopList() {
	return copList;
    }

    public void setCopList(LinkedList<WaypointPolice> copList) {
	this.copList = copList;
    }

    public LinkedList<WaypointGangster> getGangsterList() {
	return gangsterList;
    }

    public void setGangsterList(LinkedList<WaypointGangster> gangsterList) {
	this.gangsterList = gangsterList;
    }

    public LinkedList<WaypointCaught> getCaughtList() {
	return caughtList;
    }

    public void setCaughtList(LinkedList<WaypointCaught> caughtList) {
	this.caughtList = caughtList;
    }

    public LinkedList<DefaultWaypoint> getDefaultList() {
	return defaultList;
    }

    public void setDefaultList(LinkedList<DefaultWaypoint> defaultList) {
	this.defaultList = defaultList;
    }

}