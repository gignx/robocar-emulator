package justine.robocar;

import java.util.LinkedList;

public class UpdateThread extends Thread {

    private Traffic traffic = null;
    boolean interrupted = false;
    Drawer drawer = null;
    boolean need = true;

    public UpdateThread() {
	super();
    }

    public synchronized void setTraffic(Traffic t) {
	traffic = t;
    }

    public interface Drawer {
	public void onUpdated(Traffic traffic);
    }

    @Override
    public void run() {
	while (!interrupted) {
	    if (traffic == null)
		continue;
	    if (need) {
		for (LinkedList<? extends UpdateableWaypoint> list : traffic.lists) {
		    for (UpdateableWaypoint waypoint : list) {
			if (waypoint.isOnScreen())
			    waypoint.update();
		    }
		}
	    }
	    if (drawer != null)
		drawer.onUpdated(traffic);

	    try {
		Thread.sleep(10);
	    } catch (InterruptedException e) {
		e.printStackTrace();
	    }
	}
    }

    @Override
    public void interrupt() {
	interrupted = true;
	super.interrupt();
    }

}
