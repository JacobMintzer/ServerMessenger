using System;
using System.Threading;
using System.IO;
using System.Net.Sockets;
using System.Windows.Forms;
using System.Diagnostics;
using Windows.Data.Xml.Dom;
using Windows.UI.Notifications;

namespace ServerListener
{
	class ErrorMessage
	{
		public String error="";
		public void Append(String toAdd)
		{
			error += toAdd;
		}
		public void CheckChanges()
		{
			Console.WriteLine("checker launched");
			while (true)
			{
				try
				{
					if (error.Length > 2)
					{
						error = "";
						Thread.Sleep(3000);
						Notify();
					}
					else
					{

						Thread.Sleep(30000);

					}
				}
				catch (Exception e)
				{
					Console.WriteLine("error " + e);
				}
			}
		}
		public void Notify()
		{

			//XmlDocument toastXml = ToastNotificationManager.GetTemplateContent(ToastTemplateType.ToastImageAndText01);
			//XmlNodeList stringElements = toastXml.GetElementsByTagName("text");
			//stringElements[0].AppendChild(toastXml.CreateTextNode(message));
			//XmlNodeList imageElements = toastXml.GetElementsByTagName(@"D:\Documents\Rainmeter\Skins\NeneListener\@Resources\nene-spook.jpg");
			Console.WriteLine("about to set popup");
			DialogResult result = MessageBox.Show(new Form() { TopMost = true },"error in Twitter Listener, want to see the error?", "error", MessageBoxButtons.YesNo);
			if (result == System.Windows.Forms.DialogResult.Yes)
			{
				Process.Start(@"C:\Users\Noel Kannagi\Documents\code\ServerListener\errorlog.txt");
			}
		}

	}

	class TCP
	{
		
		
	  
		public NetworkStream GetStream(Int32 port, string server)
		{
			TcpClient client = new TcpClient(server, port);
			NetworkStream stream = client.GetStream();
			return stream;
		}
		static void Main(string[] args)
		{
			TCP watcher = new TCP();
			Int32 port = 8123;
			String server = System.IO.File.ReadAllText(@"C:\Users\Noel Kannagi\Documents\code\ServerListener\IP.txt");
			Boolean fail = false;
			NetworkStream stream = watcher.GetStream(port, server);
			Console.WriteLine("stream created");
			ErrorMessage err = new ErrorMessage();
			System.Threading.Thread notificationThread = new Thread(err.CheckChanges);
			notificationThread.Start();
			while (true)
			{
				try
				{
					if (fail)
					{
						stream = watcher.GetStream(port, server);
						fail = false;
					}
					Byte[] data = new Byte[256];
					Console.WriteLine("blocking on read");
					Int32 bytes = stream.Read(data, 0, data.Length);
					string message = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
					if (message.Length > 2)
					{
						Console.WriteLine("grabbed string " + message);
						using (StreamWriter sw = File.AppendText(@"C:\Users\Noel Kannagi\Documents\code\ServerListener\errorlog.txt"))
						{
							sw.WriteLine(message);
						}
						err.Append(message);
					}
					else
					{
						Console.WriteLine("read failed/no message");
						Thread.Sleep(30000);
					}
				}
				catch (IOException e)
				{
					Console.WriteLine(format: "Socket closed, IO Exception {0}, trying to reopen", arg0: e);
					Thread.Sleep(30000);
					fail = true;
				}
				catch(ObjectDisposedException e)
				{
					Console.WriteLine(format: "NetworkStream closed, Exception {0}, trying to reopen", arg0: e);
					Thread.Sleep(30000);
					fail = true;
				}
			}

		}
	}
}
