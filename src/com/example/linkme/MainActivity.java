package com.example.linkme;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.ClipboardManager;
import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.*;
import android.content.*;

/**
 * LinkMe
 * 
 * @author saadupaadu
 *
 */

public class MainActivity extends Activity {
	
	private EditText ipAddr; // Edit View to hold the IP Address
	private EditText portNumbr; // Edit View to hold the port number
	private EditText userName; // Edit View to hold the username
	private EditText password; // Edit View to hold the password
	private TextView status; // Text View to hold the status
	private EditText filename; // Edit View to hold the filename
	private String fileName; // String value of the filename
	private String fileContent; // String to hold the file contents
	private Socket s;	// Socket for the connection
	byte[] readBuffer; // Byte array for read buffer

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
	}

	
	/**
	 * Button listener for Submit Button
	 * 
	 * Takes values from the Edit Views and assigns them to appropriate strings
	 * and starts the task to create socket connection.
	 */
	public void sendFeedback(View button) {

		ipAddr = (EditText) findViewById(R.id.editTextIP);
		String ipAdd = ipAddr.getText().toString();

		portNumbr = (EditText) findViewById(R.id.editTextPort);
		String portNum = portNumbr.getText().toString();

		userName = (EditText) findViewById(R.id.editTextUN);
		String username = userName.getText().toString();

		password = (EditText) findViewById(R.id.editTextPW);
		String pass = password.getText().toString();

		status = (TextView) findViewById(R.id.statusView);

		filename = (EditText) findViewById(R.id.editTextFN);
		fileName = filename.getText().toString();

		new MakeSocketTask().execute(ipAdd, portNum, username, pass);

	}
	
	private class MakeSocketTask extends AsyncTask<String, Void, String> {
		
		protected String doInBackground(String... urls) {
			try {
				
				//establish socket
				s = new Socket(urls[0], Integer.parseInt(urls[1]));
				
				// outgoing stream redirect to socket
				PrintWriter output = new PrintWriter((new BufferedWriter(
						new OutputStreamWriter(s.getOutputStream()))), true);
				
				// sending authorization request to server
				output.println("Authorize");
				
				
				// username+password string
				String challenge = urls[2] + urls[3];
				
				// send username+password to Server
				output.println(challenge);
				
				// incoming stream from Server
				char[] buffer = new char[4];
				BufferedReader input = new BufferedReader(
						new InputStreamReader(s.getInputStream()));	
				
				
				// receive login result
				input.read(buffer);
				String check = new String(buffer);
				Log.v("Buffer Received", check);
				
				// checking if authorization was a success
				if (check.compareTo("Pass") == 0) {

					// send filename to Server
					output.println(fileName);

					// read content of file
					// stored in "fileContent"
					readBuffer = new byte[1];
					fileContent = "";
					int count = 0;
					while (s.getInputStream().read(readBuffer) != -1) {
						fileContent += (char) readBuffer[0];
						count++;
					}
					
					
					// checking if file was found
					if (count == 0) {
						return "File Not Found";
					}
					
					Log.v("File Received", fileContent);


					System.out.println(fileContent);

				} else {
					// Close connection
					s.close();
					return "Invalid Login";
				}

				// Close connection
				s.close();

				return "Success! Here is your link: \n\n" + fileContent;
				

			}catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			// Close connection
			try {
				s.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return "Connection Error";
		}
		
		protected void onPostExecute(String result) {
			status.setText(result);

		}
		
	}
	
}
