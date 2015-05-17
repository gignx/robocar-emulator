package justine.robocar;

import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;


public class JSeekbar extends JSlider implements ChangeListener {

	private static final long serialVersionUID = 1L;

	PlayBack playback;
	boolean byHuman = false;

	public JSeekbar(PlayBack p) {
		super();
		addChangeListener(this);
		playback = p;
		setValue(0);
	}

	@Override
	public void setValue(int n) {
		byHuman = false;
		super.setValue(n);
		byHuman = true;
	}

	public void stateChanged(ChangeEvent e) {
		if (byHuman) {
			int p = ((JSeekbar) e.getSource()).getValue();
			long pos = CarWindow.seekmap.get((long) p);
			playback.seekTo(pos);
		}
	}

}
