namespace NetDebugTool
{
    using System;
    using System.Collections.Generic;
    using System.Runtime.CompilerServices;

    public interface ISendUC
    {
        event EventHandler ClientToSendChanged;

        IList<int> GetClientsForSend();
        void Send(byte[] data);
        void SetLog(LogData logdata);

        bool IsStarted { get; }
    }
}

