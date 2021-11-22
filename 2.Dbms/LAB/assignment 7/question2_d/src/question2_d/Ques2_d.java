package question2_d;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Scanner;

public class Ques2_d {

	public static void main(String[] args) throws SQLException {
		// TODO Auto-generated method stub
		
		Connection connect = null;
		Statement statement = null;
		ResultSet resultSet = null;
		
		 connect= DriverManager.getConnection("jdbc:mysql://localhost:3306/assignment1","root" , "raj123SHANPK45");
		
		 statement = connect.createStatement();
		 resultSet = statement.executeQuery("select fname from faculty f , class c where f.fid = c.fid and f.deptid = 20 and c.room ='R128' ");
		 
		 System.out.println("Faculty:");
		 while(resultSet.next())
		 {
			 String fname = resultSet.getString("fname");
			 
			 System.out.println(" " +fname );
		 }
		 
		 
		 
		 
		 resultSet.close();
		 statement.close();
		 connect.close();
		 System.out.println("\nSuccessfully ------done!"); 
	}

}
