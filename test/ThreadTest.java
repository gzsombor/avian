import avian.Machine;

public class ThreadTest implements Runnable {

	boolean useThreadAllocator;

	Long finalValue;
	Long finalValue2;

	public ThreadTest(boolean f) {
		this.useThreadAllocator = f;
	}

	@Override
	public void run() {
		finalValue = new Long(5);
		if (useThreadAllocator) {
			Machine.setupThreadAllocator(100000);
		}

		System.out.println("long:" + finalValue + " from "
				+ Thread.currentThread().getName());

		finalValue2 = new Long(42);
		System.out.println("long 2:" + finalValue2 + " from "
				+ Thread.currentThread().getName());

	}

	public Long getFinalValue() {
		return finalValue;
	}

	public Long getFinalValue2() {
		return finalValue2;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		{
			ThreadTest t = new ThreadTest(false);
			testThread(t);
		}
		{
			ThreadTest t = new ThreadTest(true);
			testThread(t);
		}
	}

	private static void testThread(ThreadTest t) {
		Thread thread= new Thread(t, "sub-thread-"+t.useThreadAllocator);
		thread.start();
		try {
			thread.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		System.out.println("t1 : " + t.getFinalValue() + ", "
				+ t.getFinalValue2());
	}

}
