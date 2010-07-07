import avian.InvalidFieldAssignment;
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
		try {
			finalValue = new Long((long) (Math.random()*1000));
			if (useThreadAllocator) {
				Machine.setupThreadAllocator(100000);
			}
			System.out.println("long:" + finalValue + " from "
					+ Thread.currentThread().getName());
	
			finalValue2 = new Long((long) (Math.random()*1000));
			System.out.println("long 2:" + finalValue2 + " from "
					+ Thread.currentThread().getName());
		} catch (Exception e) {
			System.out.println("error : "+e);
		}
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
		System.out.println(InvalidFieldAssignment.class.getName());
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
			Thread.currentThread().sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		System.out.println(thread.getName() + " : " + t.getFinalValue() + ", "
				+ t.getFinalValue2());
	}

}
