import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;

/*
 * Given a string composed of the characters '0', '1', and '?', this program calculates
 * the total number of adjacent bit swaps required to sort all possible bit strings
 * ('?' can be a zero or a one) into nondecreasing order, modulo 1,000,000,007.
 */
public class Sequences
{
    static final int MODULUS = 1_000_000_007;

    /* Reads characters from standard input. */
    static String getInput() throws IOException
    {
        try (BufferedReader input = new BufferedReader(new InputStreamReader(System.in)))
        {
            return input.readLine();
        }
    }

    /*
     * Once the average number of inversions has been calculated, the modular exponent
     * of 2^(questionMarkCount) must be found.
     */
    static int modPow(int base, int exp, int mod)
    {
        int result = 1;
        for (int i = 1; i <= exp; i++)
        {
            result *= base;
            result %= mod;
        }
        return result;
    }

    /*
     * The accuracy of this method is based on the fact that, considering all bit
     * strings in an interval of binary numbers, the probability that a bit is on
     * (or off) is exactly one half. Therefore, each question mark can be treated
     * as a "half bit".
     */
    static double averageInversionCount(String bitString)
    {
        double sum = 0.0;
        double oneCount = 0.0;
        double questionMarkCount = 0.0;
        for (char bit: bitString.toCharArray())
        {
            switch (bit)
            {
                case '0':
                    sum += oneCount + questionMarkCount / 2;
                    break;
                case '1':
                    oneCount++;
                    break;
                case '?':
                    sum += (oneCount + questionMarkCount++ / 2) / 2;
                    break;
                default:
                    throw new IllegalArgumentException("Invalid character.");
            }
        }
        return sum;
    }

    /*
     * As ab % m = [(a % m)(b % m)] % m, the modular exponent and average number of
     * inversions can be calculated separately. Plugging them into the above equation
     * yields the final answer.
     */
    public static void main(String... args) throws IOException
    {
        String bitString = getInput();
        int exp = bitString.replaceAll("[^?]", "").length();
        double avgInversions = averageInversionCount(bitString);
        // Ensures both values are integers.
        switch (exp)
        {
            case 0:
                break;
            case 1:
                exp -= 1;
                avgInversions *= 2;
                break;
            default:
                exp -= 2;
                avgInversions *= 4;
                break;
        }
        exp = modPow(2, exp, MODULUS);
        avgInversions %= MODULUS;
        System.out.print(((long)exp * (long)avgInversions) % MODULUS);
    }
}
