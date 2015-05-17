package justine.robocar;

public interface PlayBack {

	public abstract void start_play();

	public abstract void stop_play();

	public abstract void seekTo(long position);

	public abstract void setSpeed(int speed);

	public abstract int getSpeed();

}