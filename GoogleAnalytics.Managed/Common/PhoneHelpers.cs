using System;
using System.Xml;

namespace GoogleAnalytics
{
    internal static class Helpers
    {
        public static string GetAppAttribute(string attributeName)
        {
            try
            {
                using (XmlReader xmlReader = XmlReader.Create("WMAppManifest.xml"))
                {
                    xmlReader.ReadToDescendant("App");
                    if (!xmlReader.IsStartElement())
                    {
                        throw new FormatException("WMAppManifest.xml is missing");
                    }
                    return xmlReader.GetAttribute(attributeName);
                }
            }
            catch
            {
                return null;
            }
        }
    }
}
