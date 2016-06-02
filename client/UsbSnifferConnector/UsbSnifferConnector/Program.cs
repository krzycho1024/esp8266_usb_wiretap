using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace UsbSnifferConnector
{
    class Program
    {
        static char GetChar(byte sp, byte b)
        {
            bool shift = (sp & 2) == 2 || (sp & 32) == 32;
            if (b >= 4 && b <= 29)
            {
                if (shift)
                {
                    return (char)(((byte)'A') - 4 + b);
                }
                else
                {
                    return (char)(((byte)'a') - 4 + b);
                }
            }
            if (b >= 30 && b <= 39)
            {
                if (shift)
                {
                    return '?';
                }
                else
                {
                    if (b == 39)
                    {
                        return '0';
                    }
                    else
                    {
                        return (char)(((byte)'1') - 30 + b);
                    }
                }
            }
            if (b == 44)
            {
                return ' ';
            }
            if (b == 40 || b == 88)
            {
                return '\n';
            }
            return '?';
        }

        static string GetKeys(byte[] buf)
        {
            StringBuilder  sb= new StringBuilder();
            if (buf[0] == 0x80 && (buf[1] == 195 || buf[1] == 75))
            {
                for (int i = 4; i < 10; i++)
                {
                    if (buf[i] > 0)
                    {
                        sb.Append(GetChar(buf[2], buf[i]));
                    }
                }
            }
            return sb.ToString();
        }

        static void Main(string[] args)
        {
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.Connect("10.10.10.1", 1080);
            while (true)
            {
                byte[] buf = new byte[16];
                int ret = socket.Receive(buf);
                if (ret > 0)
                {
                    //Console.Write(GetKeys(buf));
                    Console.WriteLine(buf[0] + " " + buf[1] + " | " +  buf[2] + " " + buf[3] + " | " + buf[4] + " " + buf[5] + " " + buf[6] + " " + buf[7] + " " + buf[8] + " " + buf[9] + " " + buf[10] + " " + buf[11] + " " + buf[12] + " " + buf[13] + " " + buf[14] + " " + buf[15] + " " + GetKeys(buf));
                }
            }

        }
    }
}
