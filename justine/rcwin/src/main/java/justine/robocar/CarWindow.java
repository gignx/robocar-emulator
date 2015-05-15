/**
 * @brief Justine Car Window (monitor)
 *
 * @file CarWindow.java
 * @author Norbert Bátfai <nbatfai@gmail.com>
 * @version 0.0.16
 *
 * @section LICENSE
 *
 * Copyright (C) 2014 Norbert Bátfai, batfai.norbert@inf.unideb.hu
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 *
 * Justine - this is a rapid prototype for development of Robocar City Emulator
 * Justine Car Window (a monitor program for Robocar City Emulator)
 *
 */
package justine.robocar;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.Map;

import javax.swing.JFrame;

import justine.robocar.NetworkThread.OnNewTrafficListener;
import justine.robocar.UpdateThread.Drawer;

import org.jxmapviewer.viewer.GeoPosition;

@SuppressWarnings("serial")
public class CarWindow extends JFrame implements OnNewTrafficListener, Drawer,
		WindowListener {

	CarPainter carPainter;
	MapViewer jXMapViewer;
	Map<Long, Loc> lmap = null;
	String hostname = "localhost";
	int port = 10007;
	private NetworkThread network;
	Traffic currentTraffic = null;

	public CarWindow(String hostname, int port) {
		this.hostname = hostname;
		this.port = port;
		addWindowListener(this);

		carPainter = new CarPainter();
		jXMapViewer = new MapViewer(carPainter);

		add(jXMapViewer);

		Dimension screenDim = Toolkit.getDefaultToolkit().getScreenSize();
		setSize(screenDim.width / 2, screenDim.height / 2);

		network = new NetworkThread(hostname, port);
		network.setOnNewTrafficListener(this);
		network.start();

		updater = new UpdateThread();
		updater.drawer = this;
	}

	public synchronized void onUpdated(Traffic traffic) {
		if (traffic.timestamp != currentTraffic.timestamp) {
			updater.setTraffic(Traffic.create(currentTraffic));
		} else {
			currentTraffic = Traffic.create(traffic);
			carPainter.update(currentTraffic);
			jXMapViewer.repaint();
			UpdateableWaypoint.max = (int) (20 / (double) (jXMapViewer
					.getZoom() + 1) + 5);

			if (jXMapViewer.getZoom() > 8)
				updater.need = false;
			else
				updater.need = true;
		}
	}

	public synchronized void onNewTraffic(Traffic traffic) {
		currentTraffic = Traffic.create(traffic);
		setTitle(currentTraffic.title);
		if (!updater.isAlive()) {
			jXMapViewer.setCenterPosition(new GeoPosition(NetworkThread.lmap
					.values().iterator().next().lat, NetworkThread.lmap
					.values().iterator().next().lon));
			jXMapViewer.setZoom(8);
			updater.setTraffic(currentTraffic);
			updater.start();
		}
	}

	public static String file;
	private UpdateThread updater;

	public static void main(String[] args) {

		String hostname = "localhost";
		int port = 10007;
		file = args[0];

		switch (args.length) {
		case 3:
			port = Integer.parseInt(args[2]);
		case 2:
			hostname = args[1];
		case 1:
			break;
		default:
			System.out
					.println("java -jar target/site/justine-rcwin-0.0.16-jar-with-dependencies.jar ../../../lmap.txt localhost");
			return;
		}
		new CarWindow(hostname, port).setVisible(true);
	}

	public void windowActivated(WindowEvent e) {
		// TODO Auto-generated method stub

	}

	public void windowClosed(WindowEvent e) {
		// TODO Auto-generated method stub

	}

	public void windowClosing(WindowEvent e) {
		System.exit(0);
	}

	public void windowDeactivated(WindowEvent e) {
		// TODO Auto-generated method stub

	}

	public void windowDeiconified(WindowEvent e) {
		// TODO Auto-generated method stub

	}

	public void windowIconified(WindowEvent e) {
		// TODO Auto-generated method stub

	}

	public void windowOpened(WindowEvent e) {
		// TODO Auto-generated method stub

	}

}
