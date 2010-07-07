package avian;

public class InvalidFieldAssignment extends RuntimeException {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public InvalidFieldAssignment(String message, Throwable cause) {
		super(message, cause);
	}

	public InvalidFieldAssignment(String message) {
		super(message);
	}

	public InvalidFieldAssignment(Throwable cause) {
		super(cause);
	}

	public InvalidFieldAssignment() {
	}

}
