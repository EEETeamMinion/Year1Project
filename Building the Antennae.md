
 _First you will need to pick up the radio waves via an antenna. You can use a tuned coil antenna, which is essentially an air-cored inductor with a reasonably large diameter that you can make by coiling wire._

We checked the inductance using the LCR meter in the labs and achieved $826.4 \mu H$. We're trying to optimise for the 89kHz resonant frequency so we can find the optimal value for capacitance as shown below.

$$ \begin{aligned} f_0=89kHz
\\
L=826.4\micro H
\\
f_0=\frac{1}{2\pi \sqrt{LC}}
\\
89kHz=\frac{1}{2\pi \sqrt{826.4\mu \cdot C}}
\\
C=3.87nF
\end{aligned}
$$
 ![[Pasted image 20260513214651.png]]
Here's a picture of our tuned circuit for the resonant frequency of 89kHz.

**Why did we make a parallel resonant circuit instead of a series resonant circuit?**

We noticed that in series, the circuit was much more susceptable to noise, however in parallel we were able to pick up a clean sine wave. 

- In parallel, the impedance is at it's maximum at the resonant frequency
- In series, the impedance is at it's minimum at the resonant frequency
Noise is shunted to ground in parallel however in series, noise is kept in the loop. 

![[Oscilloscope Reading.png]]

As we can see, when the rock is placed near the antennae, a pure sine wave is seen at a frequency of ~89kHz!

