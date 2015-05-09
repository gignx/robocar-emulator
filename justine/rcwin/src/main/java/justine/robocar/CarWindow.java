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
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.swing.JFrame;

import justine.robocar.BackgroundThread.NotifyListener;


@SuppressWarnings("serial")
public class CarWindow extends JFrame implements NotifyListener {

    CarPainter carPainter;
    MapViewer jXMapViewer;
    Map<Long, Loc> lmap = null;
    String hostname = "localhost";
    int port = 10007;
    private Thread worker;

    public CarWindow(double lat, double lon, Map<Long, Loc> lmap, String hostname, int port) {
	this.lmap = lmap;
	this.hostname = hostname;
	this.port = port;

	carPainter = new CarPainter();
	jXMapViewer = new MapViewer(lat, lon, carPainter);
	add(jXMapViewer);

	Dimension screenDim = Toolkit.getDefaultToolkit().getScreenSize();
	setSize(screenDim.width / 2, screenDim.height / 2);

	worker = new Thread(new BackgroundThread(lmap, hostname, port, this));
	worker.start();

	addWindowListener(new WindowAdapter() {

	    public void windowClosed(WindowEvent e) {
		worker.stop();
		e.getWindow().dispose();
	    }

	});
    }

    public void onDataChanged(Traffic traffic) {
	setTitle(traffic.title);
	carPainter.update(traffic);
	jXMapViewer.repaint();
    }

    public void onException(Exception e) {
	System.out.println(e.getMessage());
	System.exit(1);
    }

    public static void readMap(Map<Long, Loc> lmap, String name) {

	Scanner scan;
	java.io.File file = new java.io.File(name);

	long ref = 0;
	double lat = 0.0, lon = 0.0;
	try {

	    scan = new Scanner(file);

	    while (scan.hasNext()) {

		ref = scan.nextLong();
		lat = scan.nextDouble();
		lon = scan.nextDouble();

		lmap.put(ref, new Loc(lat, lon));
	    }

	} catch (Exception e) {

	    Logger.getLogger(CarWindow.class.getName()).log(Level.SEVERE, "hibás noderef2GPS leképezés", e);

	}

    }

    public static void main(String[] args) {

	final Map<Long, Loc> lmap = new HashMap<Long, Loc>();
	readMap(lmap, args[0]);
	String hostname = "localhost";
	int port = 10007;

	switch (args.length) {
	case 3:
	    port = Integer.parseInt(args[2]);
	case 2:
	    hostname = args[1];
	case 1:
	    break;
	default:
	    System.out.println("java -jar target/site/justine-rcwin-0.0.16-jar-with-dependencies.jar ../../../lmap.txt localhost");
	    return;
	}
	Map.Entry<Long, Loc> e = lmap.entrySet().iterator().next();
	new CarWindow(e.getValue().lat, e.getValue().lon, lmap, hostname, port).setVisible(true);
    }

}
