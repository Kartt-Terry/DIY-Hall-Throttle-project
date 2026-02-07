using System;
using System.IO.Ports;
using System.Text;

namespace ThrottleUI.Services
{
    /// <summary>
    /// Eritt‰in pieni sarjaporttipalvelu:
    /// - Open(port, baud) avaa portin
    /// - LineReceived laukeaa jokaisesta rivist‰ (\\n)
    /// - Send(line) l‰hett‰‰ rivin (WriteLine)
    /// - Dispose sulkee portin
    /// </summary>
    public class SerialService : IDisposable
    {
        private SerialPort? _port;
        public event Action<string>? LineReceived;

        public void Open(string portName, int baud = 115200)
        {
            // Huom: NewLine = "\n" on t‰rke‰, koska ReadLine() odottaa sit‰.
            _port = new SerialPort(portName, baud)
            {
                NewLine = "\n",
                Encoding = Encoding.ASCII
            };

            _port.DataReceived += (s, e) =>
            {
                try
                {
                    // Luetaan yksi rivi kerrallaan. Jos rivinvaihtoa ei tule, t‰m‰ blokkaa lyhyesti.
                    var line = _port!.ReadLine();
                    LineReceived?.Invoke(line);
                }
                catch
                {
                    // Nielaistaan yksitt‰iset lukuvirheet (portti kiinni tms.)
                }
            };

            _port.Open();
        }

        public void Send(string line)
        {
            if (_port?.IsOpen == true)
            {
                // WriteLine lis‰‰ NewLine-merkin automaattisesti
                _port.WriteLine(line);
            }
        }

        public void Dispose()
        {
            try { _port?.Close(); } catch { /* ignore */ }
            _port = null;
        }
    }
}