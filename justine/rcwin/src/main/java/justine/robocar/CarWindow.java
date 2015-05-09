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
import java.util.Map;

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

    public CarWindow(String hostname, int port) {
	this.hostname = hostname;
	this.port = port;

	carPainter = new CarPainter();
	jXMapViewer = new MapViewer(carPainter);
	
	
	add(jXMapViewer);

	Dimension screenDim = Toolkit.getDefaultToolkit().getScreenSize();
	setSize(screenDim.width / 2, screenDim.height / 2);

	worker = new Thread(new BackgroundThread(hostname, port, this));
	worker.start();

	addWindowListener(new WindowAdapter() {

	    public void windowClosed(WindowEvent e) {
		worker.interrupt();
		try {
		    worker.join();
		} catch (InterruptedException e1) {
		    e1.printStackTrace();
		}
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

    public static String file;

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
	    System.out.println("java -jar target/site/justine-rcwin-0.0.16-jar-with-dependencies.jar ../../../lmap.txt localhost");
	    return;
	}
	new CarWindow(hostname, port).setVisible(true);
    }

}
