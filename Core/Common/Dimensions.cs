
using System;
namespace GoogleAnalytics.Core
{
    public sealed class Dimensions
    {
        public Dimensions(int width, int height)
        {
            Width = width;
            Height = height;
        }

        public int Width { get; set; }

        public int Height { get; set; }

        //public static bool operator ==(Dimensions d1, Dimensions d2)
        //{
        //    return d1.Width == d2.Width && d1.Height == d2.Height;
        //}

        //public static bool operator !=(Dimensions d1, Dimensions d2)
        //{
        //    return d1.Width != d2.Width || d1.Height != d2.Height;
        //}

        //public override bool Equals(object obj)
        //{
        //    return ((Dimensions)obj).Width == Width && ((Dimensions)obj).Height == Height;
        //}

        //public override int GetHashCode()
        //{
        //    return ShiftAndWrap(Width.GetHashCode(), 2) ^ Height.GetHashCode();
        //}

        //static int ShiftAndWrap(int value, int positions)
        //{
        //    positions = positions & 0x1F;

        //    // Save the existing bit pattern, but interpret it as an unsigned integer. 
        //    uint number = BitConverter.ToUInt32(BitConverter.GetBytes(value), 0);
        //    // Preserve the bits to be discarded. 
        //    uint wrapped = number >> (32 - positions);
        //    // Shift and wrap the discarded bits. 
        //    return BitConverter.ToInt32(BitConverter.GetBytes((number << positions) | wrapped), 0);
        //}

    }
}
