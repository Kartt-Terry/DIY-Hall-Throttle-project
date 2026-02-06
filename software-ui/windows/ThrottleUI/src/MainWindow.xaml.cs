using System.Windows;

namespace ThrottleUI
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            MainContent.Content = new Views.DrivingView();
        }

        private void BtnDriving_Click(object sender, RoutedEventArgs e) => MainContent.Content = new Views.DrivingView();
        private void BtnSettings_Click(object sender, RoutedEventArgs e) => MainContent.Content = new Views.SettingsView();
        private void BtnFault_Click(object sender, RoutedEventArgs e) => MainContent.Content = new Views.FaultView();
        private void BtnDiagnostics_Click(object sender, RoutedEventArgs e) => MainContent.Content = new Views.DiagnosticsView();
    }
}
