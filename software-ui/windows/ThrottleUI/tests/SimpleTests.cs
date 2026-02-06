using Xunit;
using FluentAssertions;

namespace ThrottleUI.Tests
{
    public class SimpleTests
    {
        [Fact]
        public void Math_should_work()
        {
            (2 + 2).Should().Be(4);
        }
    }
}
