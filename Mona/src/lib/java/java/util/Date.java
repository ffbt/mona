package java.util;

public class Date {
	private int year, month, date, hour, minutes, second;
	
	private native void create(Date date);
	
	public Date() {
		create(this);
	}
	
	public int getYear() {
		return this.year;
	}
	
	public int getMonth() {
		return this.month;
	}
	
	public int getDate() {
		return this.date;
	}
	
	public int getHours() {
		return this.hour;
	}
	
	public int getMinutes() {
		return this.minutes;
	}
	
	public int getSeconds() {
		return this.second;
	}
	
	public int getDay() {
		int y = this.year + 1900;
		int m = this.month + 1;
		int d = this.date;
		
		if ((m == 1) || (m == 2)) {
			y--;
			m += 12;
		}
		
		return ((y + (int)(y/4) - (int)(y/100) + (int)(y/400) + (int)((13 * m + 8)/5) + d) % 7);
	}
}