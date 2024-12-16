#!//home/mead/epics/pscdrv/bin/linux-x86_64/pscdemo 
epicsEnvSet("TOP","/home/mead/epics/pscdrv")
epicsEnvSet("BPMDIR","$(TOP)")
epicsEnvSet("ZUBPM_DBDIR","/home/mead/rfbpm/fwk/rfsoc-dfe/ioc")


#epicsEnvSet("CNO","40")   ## Cell Number
#epicsEnvSet("HOSTNAME","diagioc-c$(CNO)")
epicsEnvSet("IOCNAME", "LN-BI")





###
epicsEnvSet("EPICS_CA_AUTO_ADDR_LIST", "YES")
#epicsEnvSet("EPICS_CA_ADDR_LIST", "10.0.142.20")
epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES", "16000000")


## You may have to change psc to something else
## everywhere it appears in this file

## Register all support components
dbLoadDatabase("$(BPMDIR)/dbd/pscdemo.dbd",0,0)
pscdemo_registerRecordDeviceDriver(pdbbase) 

# BPM IP address
epicsEnvSet("BPM1_IP", "10.0.142.10");  #4009


## Load record instances
epicsEnvSet("ALIVELEN", "8000");   # ADC Live length
epicsEnvSet("TLIVELEN", "8000");   # ADC Live length
epicsEnvSet("ALEN",1000000);        # ADC DMA Length
epicsEnvSet("TLEN", "100000");      # TbT Length

########## use template

### PVs for first LINAC ZBPM:
dbLoadRecords("$(ZUBPM_DBDIR)/rfsocbpm.db", "P=$(IOCNAME), NO=1, ADC_LIVE_WFM_LEN=$(ALIVELEN), ADC_WFM_LEN=$(ALEN), TBT_LIVE_WFM_LEN=$(TLIVELEN), TBT_WFM_LEN=$(TLEN), SAVG_N=100, SAVG_NSAM=100")
dbLoadRecords("$(ZUBPM_DBDIR)/SFP.db", "P=$(IOCNAME), B=1, S=0, TMP=140, VCC=164, TXB=188, TXP=212, RXP=236")
dbLoadRecords("$(ZUBPM_DBDIR)/SFP.db", "P=$(IOCNAME), B=1, S=1, TMP=144, VCC=168, TXB=192, TXP=216, RXP=240")
dbLoadRecords("$(ZUBPM_DBDIR)/SFP.db", "P=$(IOCNAME), B=1, S=2, TMP=148, VCC=172, TXB=196, TXP=220, RXP=244")
dbLoadRecords("$(ZUBPM_DBDIR)/SFP.db", "P=$(IOCNAME), B=1, S=3, TMP=152, VCC=176, TXB=200, TXP=224, RXP=248")
dbLoadRecords("$(ZUBPM_DBDIR)/SFP.db", "P=$(IOCNAME), B=1, S=4, TMP=156, VCC=180, TXB=204, TXP=228, RXP=252")
dbLoadRecords("$(ZUBPM_DBDIR)/SFP.db", "P=$(IOCNAME), B=1, S=5, TMP=160, VCC=184, TXB=208, TXP=232, RXP=256")
#dbLoadRecords("$(ZBPM_DBDIR)/PowMan.db", "P=$(IOCNAME), NO=1")

#####################################################
var(PSCDebug, 5)	#5 full debug

#bpm1 Create the PSC
createPSC("aie_tx_1", $(BPM1_IP), 7, 0)
createPSC("aie_wfm_rx_1", $(BPM1_IP), 20, 2)
createPSC("Rx1", $(BPM1_IP), 600, 2)

###########
iocInit
###########

#epicsThreadSleep 1

#dbpf $(IOCNAME){BPM:1}Gain:Adc0-SP, 32767
#dbpf $(IOCNAME){BPM:1}Gain:Adc1-SP, 32767
#dbpf $(IOCNAME){BPM:1}Gain:Adc2-SP, 32767
#dbpf $(IOCNAME){BPM:1}Gain:Adc3-SP, 32767

#dbpf $(IOCNAME){BPM:1}Gain:RfAtte-SP, 0 






