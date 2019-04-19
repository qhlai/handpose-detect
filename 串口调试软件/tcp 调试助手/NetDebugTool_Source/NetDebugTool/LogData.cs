namespace NetDebugTool
{
    using System;

    public class LogData
    {
        public byte[] Data;
        public int Index;
        public string IP;
        public bool IsTcpClient;
        public string Log;
        public int Loglevel;
        public int Port;
        public DateTime Time;

        public LogData(string log, int loglevel)
        {
            this.Log = log;
            this.Loglevel = loglevel;
        }

        public LogData(byte[] data, int index, string ip, bool isTcpClient, string log, int port, DateTime time, int loglevel)
        {
            this.Data = data;
            this.Index = index;
            this.IP = ip;
            this.IsTcpClient = isTcpClient;
            this.Log = log;
            this.Port = port;
            this.Time = time;
            this.Loglevel = loglevel;
        }
    }
}

