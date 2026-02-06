using System.Windows.Controls;

namespace ThrottleUI.Views
{
    public partial class DrivingView : UserControl
    {
        public DrivingView()
        {
            InitializeComponent();
            // TODO: bind to telemetry; placeholder demo value
            Bar.Value = 0;
            LblPercent.Text = "0 %";
        }
    }
}
