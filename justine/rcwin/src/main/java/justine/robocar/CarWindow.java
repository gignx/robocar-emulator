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

import java.awt.Button;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Toolkit;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;

import justine.robocar.NetworkThread.OnNewTrafficListener;
import justine.robocar.TrafficStateProtos.TrafficStateHeader;
import justine.robocar.UpdateThread.Drawer;

import org.jxmapviewer.viewer.GeoPosition;

@SuppressWarnings("serial")
public class CarWindow extends JFrame implements OnNewTrafficListener, Drawer, WindowListener {

	CarPainter carPainter;
	MapViewer jXMapViewer;
	Map<Long, Loc> lmap = null;
	String hostname = "localhost";
	int port = 10007;
	private NetworkThread network;
	Traffic currentTraffic = null;
	JLabel maxtime;
	JLabel timenow;
	JSeekbar seeker;
	Button play;
	Button speed_minus;
	Button speed_plus;
	JLabel speed;
	PlayBack playback;

	public abstract interface PlayBack {
		public void start_play();

		public void stop_play();

		public void seekTo(long position);

		public void setSpeed(int speed);

		public int getSpeed();
	}

	public CarWindow(String hostname, int port) {
		this.hostname = hostname;
		this.port = port;
		addWindowListener(this);
		network = new NetworkThread(hostname, port);
		network.setOnNewTrafficListener(this);
		playback = network;

		carPainter = new CarPainter();
		jXMapViewer = new MapViewer(carPainter);
		if (log) {
			GridBagLayout gl = new GridBagLayout();
			GridBagConstraints cc = new GridBagConstraints();
			setLayout(gl);

			cc.fill = GridBagConstraints.BOTH;
			cc.weightx = 10;
			cc.weighty = 10;
			cc.gridwidth = GridBagConstraints.REMAINDER;
			add(jXMapViewer, cc);
			JPanel panel = new JPanel();
			panel.setBorder(new CompoundBorder(BorderFactory.createLineBorder(Color.BLACK, 2), new EmptyBorder(10, 10, 10, 10)));
			cc.fill = GridBagConstraints.HORIZONTAL;
			cc.weightx = 0;
			cc.weighty = 0;
			add(panel, cc);

			panel.setLayout(new GridBagLayout());
			GridBagConstraints c = new GridBagConstraints();
			seeker = new JSeekbar(playback);
			play = new Button("PAUSE");
			play.addMouseListener(new MouseAdapter() {
				boolean playing = true;

				public void mouseClicked(MouseEvent e) {
					if (playing) {
						playback.stop_play();
						play.setLabel("PLAY");
					} else {
						playback.start_play();
						play.setLabel("PAUSE");
					}
					playing = !playing;
				}

			});
			speed_minus = new Button("-");
			speed_minus.addMouseListener(new MouseAdapter() {
				public void mouseClicked(MouseEvent e) {
					playback.setSpeed(playback.getSpeed() + 10);
					speed.setText(String.format("%.2fx", 200 / (double) playback.getSpeed()));
				}
			});
			speed_plus = new Button("+");
			speed_plus.addMouseListener(new MouseAdapter() {
				public void mouseClicked(MouseEvent e) {
					playback.setSpeed(playback.getSpeed() - 10);
					speed.setText(String.format("%.2fx", 200 / (double) playback.getSpeed()));
				}
			});

			speed = new JLabel("1.00x");
			speed_minus.setPreferredSize(new Dimension(24, 24));
			speed_plus.setPreferredSize(new Dimension(24, 24));

			c.fill = GridBagConstraints.HORIZONTAL;
			panel.add(play, c);
			panel.add(speed_minus, c);
			panel.add(speed, c);
			panel.add(speed_plus, c);

			timenow = new JLabel("01:15");
			panel.add(timenow);
			c.gridwidth = GridBagConstraints.RELATIVE;
			c.fill = GridBagConstraints.HORIZONTAL;
			c.weightx = 1;
			panel.add(seeker, c);
			maxtime = new JLabel("06:23");
			panel.add(maxtime);
			try {
				preProccesLog();
				seeker.setMaximum((int) length);
				StringBuilder sb = new StringBuilder(10);
				long min = length / 60;
				int s = (int) (length % 60);
				if (min < 10)
					sb.append("0");
				sb.append(min);
				sb.append(":");
				if (s < 10)
					sb.append("0");
				sb.append(s);
				maxtime.setText(sb.toString());
			} catch (IOException e) {
				e.printStackTrace();
			}
		} else {
			add(jXMapViewer);
		}
		Dimension screenDim = Toolkit.getDefaultToolkit().getScreenSize();
		setSize(screenDim.width / 2, screenDim.height / 2);

		network.start();

		updater = new UpdateThread();
		updater.drawer = this;
	}

	public static Map<Long, Long> seekmap = new HashMap<Long, Long>();
	long length;

	public void preProccesLog() throws IOException {
		File f = new File(CarWindow.logfile);
		FileInputStream is = new FileInputStream(f);
		BufferedInputStream bis = new BufferedInputStream(is);
		byte buff[] = new byte[524288];
		long position = 0;
		int toRead;
		try {
			while (bis.available() > 4) {
				bis.read(buff, 0, 4);
				toRead = java.nio.ByteBuffer.wrap(buff).order(java.nio.ByteOrder.LITTLE_ENDIAN).getInt();
				bis.read(buff, 0, toRead);
				byte[] readFrom = new byte[toRead];
				java.nio.ByteBuffer.wrap(buff).order(java.nio.ByteOrder.LITTLE_ENDIAN).get(readFrom, 0, toRead);
				TrafficStateHeader trafficStateHeader = TrafficStateHeader.parseFrom(readFrom);

				// trafficStateHeader.getTimeMinutes() ignored, because its
				// unreliable
				long time = trafficStateHeader.getTimeElapsed();
				int size = trafficStateHeader.getNumCars();

				int inter_pos = 0;

				for (int i = 0; i < size; i++) {
					bis.read(buff, 0, 4);
					int read = java.nio.ByteBuffer.wrap(buff).order(java.nio.ByteOrder.LITTLE_ENDIAN).getInt();
					// bis.skip(read) has undefined behavior, and may skip more
					// or less than specified (#justJAVAthings)
					bis.read(buff, 0, read);
					inter_pos += 4 + read;
				}
				seekmap.put(time / 5, position);
				position += 4 + toRead + inter_pos;
				length = time / 5;
			}
		} catch (IndexOutOfBoundsException e) {
			// End of logfile reached, but last entry was corrupted
			// The reason behind this could be,
			// that the traffic server was shut down before the end of the
			// simulation.
			// In this case we read the log until the last correct entry is
			// reached.
		}
		bis.close();
		is.close();
	}

	public synchronized void onUpdated(Traffic traffic) {
		if (traffic.timestamp != currentTraffic.timestamp) {
			updater.setTraffic(Traffic.create(currentTraffic));
		} else {
			currentTraffic = Traffic.create(traffic);
			carPainter.update(currentTraffic);
			jXMapViewer.repaint();
			UpdateableWaypoint.max = (int) (20 / (double) (jXMapViewer.getZoom() + 1) + 5);

			if (jXMapViewer.getZoom() > 8)
				updater.need = false;
			else
				updater.need = true;
		}
	}

	public synchronized void onNewTraffic(Traffic traffic) {
		if (currentTraffic != null && log) {
			if (traffic.seconds != currentTraffic.seconds && !seeker.getValueIsAdjusting()) {
				seeker.setValue(seeker.getValue() + 1);
				StringBuilder sb = new StringBuilder(10);
				long min = seeker.getValue() / 60;
				int s = (int) (seeker.getValue() % 60);
				if (min < 10)
					sb.append("0");
				sb.append(min);
				sb.append(":");
				if (s < 10)
					sb.append("0");
				sb.append(s);
				timenow.setText(sb.toString());
			}
		}
		currentTraffic = Traffic.create(traffic);
		setTitle(currentTraffic.title);
		if (!updater.isAlive()) {
			jXMapViewer.setCenterPosition(new GeoPosition(NetworkThread.lmap.values().iterator().next().lat, NetworkThread.lmap.values().iterator().next().lon));
			jXMapViewer.setZoom(8);
			updater.setTraffic(currentTraffic);
			updater.start();
		}
	}

	public static String file;
	public static String logfile;
	private UpdateThread updater;
	public static boolean log;

	public static void main(String[] args) {

		String hostname = "localhost";
		int port = 10007;
		file = args[0];

		if (args.length == 3 && args[1].equals("--playlog")) {
			log = true;
			logfile = args[2];
			new CarWindow(hostname, port).setVisible(true);
		} else {
			log = false;
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
