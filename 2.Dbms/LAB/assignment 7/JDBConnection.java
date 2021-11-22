import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Statement;

public class DBConnection {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
				Connection connect = null;
				Statement statement = null;
				ResultSet resultSet = null;

				try {

					// set path
					//Class.forName("com.mysql.jdbc.Driver");

					// get connection
					connect = DriverManager
							.getConnection("jdbc:mysql://localhost:3306/batch2015?user=root&password=sanjay&useSSL=false");

					// statements allow to issue SQL queries to the database
					statement = connect.createStatement();
					
					/*Fetch data from Person table*/
					resultSet = statement.executeQuery("select * from Person");
					
					System.out.println("Person Table::");
					while (resultSet.next()) {
						String userID = resultSet.getString("id");
						String userAge = resultSet.getString("age");
						System.out.println("ID : " + userID + " Age : " + userAge);
					}
					
					
					/*Fetch data from Student table*/
					resultSet = statement.executeQuery("select Student.marks,Student.id from Student,Person where Student.id = Person.id and Student.marks >=50");
					System.out.println("\nStudent Table::");
					while (resultSet.next()) {
						String userID = resultSet.getString(2);
						String userMarks = resultSet.getString(1);
						System.out.println("ID : " + userID + " Marks : " + userMarks);
					}
					
					
					statement.close();
					connect.close();
				} catch (Exception ex) {
					// handle the error
					ex.printStackTrace();
				} 
	}
}
