## Raspberry Pi HPC Cluster

### Project goals

* Create a cluster of 4 Raspberry Pi motherboards that can run OpenMPI
* Document the process such that it can be recreated easily by others
* Benchmark performance of OpenMPI on the cluster

##### Nice-to-have

* Make cluster relocatable and dynamic

### System requirements and costs

#### Hardware

* 4 x [Raspberry Pi](https://www.amazon.com/gp/product/B01EW3QU22/ref=ox_sc_act_title_2?ie=UTF8&psc=1&smid=AAU5UPIIBDRLP) ($42.99 each including heat sinks and power supply)
* 4 x [MicroSD cards](https://www.amazon.com/gp/product/B00488G6P8/ref=ox_sc_act_title_4?ie=UTF8&psc=1&smid=A35BKAA6VXTYCT) ($6.75 each)
* 4 x [ethernet patch cable](https://www.amazon.com/gp/product/B00000JDF5/ref=ox_sc_act_title_5?ie=UTF8&psc=1&smid=ATVPDKIKX0DER) ($3.60 each)
* 1 [ethernet switch](https://www.amazon.com/gp/product/B003X7TRWO/ref=ox_sc_act_title_3?ie=UTF8&psc=1&smid=ATVPDKIKX0DER) ($19.99)
* 1 [stackable case](https://www.amazon.com/gp/product/B01LVUVVOQ/ref=ox_sc_act_title_1?ie=UTF8&psc=1&smid=A2DBBGJY78ZF8D) ($22.93)

Total: $256.28 + taxes

#### Software

* OS: openSUSE or Raspbian. openSUSE is the only 64 bit ARM OS available, but Raspbian is more common. My preference is openSUSE. In the off-chance that it doesn't work, we switch to Raspbian.
* OpenMPI: Compile from source.
* Benchmarks:  
    -[OSU MPI Benchmarks](http://mvapich.cse.ohio-state.edu/benchmarks/) for micro-benchmarks  
    - Some actual code for macro-benchmarks
    - Distributed network training with pytorch. (The library is experimental, so I may need to contribute to it, or write it from stratch.)
    - Parallel/Distributed K-means. http://www.ece.northwestern.edu/~wkliao/Kmeans/index.html

### Process

1. Setup one Raspberry Pi with OS 
2. Install OpenMPI
3. Clone the disk into the other Pis
4. Create the cluster
5. Test and run benchmarks.
6. Compare the scaling with HPC and/or CIMS machines.
7. Investigate on making the cluster relocatable and easily expandable


