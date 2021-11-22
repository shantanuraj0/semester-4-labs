package mysql_p;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;


public class Javademo {

	public static void main(String[] args) throws SQLException {
		// TODO Auto-generated method stub
	

				Connection connect= DriverManager.getConnection("jdbc:mysql://localhost:3306/assignment1","root" , "raj123SHANPK45");
				Statement stmt = connect.createStatement();
				String s = "insert into person ( id , name) values ( 20 , 'raj1')";
				stmt.execute(s);
				connect.close();
				System.out.println("query is executed");
			
		
	}

}
