using System;
using System.IO.Ports;
using System.Text;
using System.Threading.Tasks;

namespace ThrottleUI.Services
{
    public class SerialService : IDisposable
    {
        private SerialPort? _port;
        public event Action<string>? LineReceived;

        public void Open(string portName, int baud = 115200)
        {
            _port = new SerialPort(portName, baud) { NewLine = "
", Encoding = Encoding.ASCII };
            _port.DataReceived += (s, e) =>
            {
                try { var line = _port!.ReadLine(); LineReceived?.Invoke(line); } catch { }
            };
            _port.Open();
        }

        public void Send(string line)
        {
            if (_port?.IsOpen == true) _port.WriteLine(line);
        }

        public void Dispose()
        {
            try { _port?.Close(); } catch { }
        }
    }
}
