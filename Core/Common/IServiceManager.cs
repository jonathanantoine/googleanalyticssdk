
namespace GoogleAnalytics.Core
{
    public interface IServiceManager
    {
        void SendPayload(Payload payload);
        string UserAgent { get; set; }
    }
}
