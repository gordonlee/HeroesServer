using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace TazanClient
{
    public class ClientObject
    {
        public Socket socket = null;
        public const int readBufferSize = 32768;
        public byte[] readBuffer = new byte[readBufferSize];
        public int readBufferLength = 0;
    }

    public class UserInfo
    {
        public int UserID;
        public int X;
        public int Y;
        public int Direction;
        public bool IsShow = true;

        public override string ToString()
        {
            return string.Format("[{0}]({1},{2})<{3}>", UserID, X, Y, Direction);
        }
    }

    class AsyncTcpClient
    {
        public Main mainForm = null;
        public Object lockObejct = new Object();

        public const string ServerHost = "127.0.0.1";
        public const int ServerPort = 9000;

        public ClientObject clientObject = new ClientObject();
        public UserInfo userInfo { get { return userInfo_; } private set { } }
        private UserInfo userInfo_ = new UserInfo();
        public List<UserInfo> otherUserInfo { get { return otherUserInfo_; } private set { } }
        private List<UserInfo> otherUserInfo_ = new List<UserInfo>();

        public AsyncTcpClient() { }
        ~AsyncTcpClient() { }

        public void Start(Main mainForm)
        {
            this.mainForm = mainForm;

            IPAddress ipAddress = Dns.Resolve(ServerHost).AddressList[0];
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, ServerPort);

            clientObject.socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            clientObject.socket.BeginConnect(remoteEP, new AsyncCallback(ConnectCallback), clientObject);
        }

        private void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                ClientObject client = (ClientObject)ar.AsyncState;

                client.socket.EndConnect(ar);

                // Connect Success
                StartReceive(client);

                // LoginRequestMessage
                BinaryWriter packetWriter = new BinaryWriter(new MemoryStream());
                packetWriter.Write((short)0);
                packetWriter.Write((byte)10);
                packetWriter.Write((byte)0x55);
                packetWriter.BaseStream.Seek(0, SeekOrigin.Begin);
                BinaryReader packetReader = new BinaryReader(packetWriter.BaseStream);
                byte[] packet = packetReader.ReadBytes(4);
                StartSend(client, packet);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public void StartReceive(ClientObject client)
        {
            try
            {
                client.socket.BeginReceive(client.readBuffer, client.readBufferLength,
                    ClientObject.readBufferSize - client.readBufferLength, SocketFlags.None,
                    new AsyncCallback(ReceiveCallback), client);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        int a = 0;
        private void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                ClientObject client = (ClientObject)ar.AsyncState;

                int readBytes = client.socket.EndReceive(ar);
                
                client.readBufferLength += readBytes;

                while (client.readBufferLength >= 4)
                {
                    // Packet Processing
                    BinaryReader packetReader = new BinaryReader(new MemoryStream(client.readBuffer));
                    short dataSize = (short)(packetReader.ReadInt16() + 4);
                    byte flag = packetReader.ReadByte();
                    byte checksum = packetReader.ReadByte();

                    if (dataSize <= client.readBufferLength)
                    {
                        if (checksum == 0x55)
                        {
                            switch (flag)
                            {
                                case 11:
                                    // LoginResultMessage
                                    {
                                        userInfo_.UserID = packetReader.ReadInt32();
                                        userInfo_.X = packetReader.ReadInt32();
                                        userInfo_.Y = packetReader.ReadInt32();
                                        userInfo_.Direction = packetReader.ReadInt32();

                                        int UserCount = packetReader.ReadInt32();
                                        for (int i = 0; i < UserCount; ++i)
                                        {
                                            UserInfo info = new UserInfo();
                                            info.UserID = packetReader.ReadInt32();
                                            info.X = packetReader.ReadInt32();
                                            info.Y = packetReader.ReadInt32();
                                            info.Direction = packetReader.ReadInt32();
                                            otherUserInfo_.Add(info);
                                        }

                                        break;
                                    }
                                case 12:
                                    // JoinNewUserMessage
                                    {
                                        UserInfo info = new UserInfo();
                                        info.UserID = packetReader.ReadInt32();
                                        info.X = packetReader.ReadInt32();
                                        info.Y = packetReader.ReadInt32();
                                        info.Direction = packetReader.ReadInt32();
                                        otherUserInfo_.Add(info);

                                        break;
                                    }
                                case 13:
                                    // ClientLeaveMessage
                                    {
                                        int LeaveUserID = packetReader.ReadInt32();
                                        foreach (var info in otherUserInfo_)
                                        {
                                            if (info.UserID == LeaveUserID)
                                            {
                                                info.IsShow = false;
                                                otherUserInfo_.Remove(info);
                                                break;
                                            }
                                        }

                                        break;
                                    }
                            }
                        }

                        // 버퍼 앞으로 당긴다.
                        client.readBufferLength -= dataSize;
                        for (int i = 0; i < client.readBufferLength; ++i)
                        {
                            client.readBuffer[i] = client.readBuffer[i + dataSize];
                        }
                    }
                }

                StartReceive(client);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void StartSend(ClientObject client, byte[] data)
        {
            try
            {
                client.socket.BeginSend(data, 0, data.Length, 0,
                    new AsyncCallback(SendCallback), client);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private void SendCallback(IAsyncResult ar)
        {
            try
            {
                ClientObject client = (ClientObject)ar.AsyncState;

                int bytesSent = client.socket.EndSend(ar);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
    }
}
