<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:gsdml="http://www.profibus.com/GSDML/2003/11/DeviceProfile" exclude-result-prefixes="gsdml">
	<xsl:param name="Language"/>
	<xsl:output method="html" encoding="UTF-8" doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN" doctype-system="http://www.w3.org/TR/html4/loose.dtd"/>

	<!-- Info about this stylesheet -->
	<xsl:variable name="DocumentInfo" select="'GSDML-Stylesheet-HTML-V2.3.xsl  2012-01-27'"/>

<!--============
Matched Templates (main and lists in //ISO15745Profile/ProfileBody/ApplicationProcess)
============-->

	<!-- Main Template -->
	<xsl:template match="/">
		<html>
			<head>
				<title><xsl:value-of select="gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:DeviceFunction/gsdml:Family/@ProductFamily"/></title>
				<style type="text/css">

 a:link { text-decoration:none; color:#294d96; }
 a:visited { text-decoration:none; color:#294d96; }
 a:hover { text-decoration:underline; color:#000000; }

h2 { font-size:12pt;
     color:#294D96;
     margin-top:20pt;
     margin-bottom:5pt;
     font-family:Arial; }

p { font-size:7pt;
     font-family:Arial;
     color:#A0A0A0; }

table { border:2px solid gray;
     background-color:#E9EDF4;
     empty-cells:show;
     border-collapse:collapse;
     margin-top:5pt;
     margin-bottom:20pt;
     font-family:Arial;
     font-weight:normal;
     text-align:left; }

th { border:1px solid gray;
     font-size:9pt;
     font-family:Arial;
     font-weight:bold;
     color:#FFFFFF;
     background-color:#294D96;
     text-align:left; }

td { border:1px solid gainsboro;
     font-size:9pt;
     font-family:Arial;
     font-weight:normal;
     color:black;
     text-align:left; }

				</style>
			</head>
			<body bgcolor="white">
				<!-- ProfileHeader and Signature are not relevant for user -->
				<xsl:apply-templates select="gsdml:ISO15745Profile/gsdml:ProfileBody"/>
				<p>Note: This page shows the content of a GSD file transformed into HTML format. In the case of disparity between this and the XML view, the content of the XML file takes precedence.<br/>Created by <xsl:value-of select="$DocumentInfo"/>.</p>
			</body>
		</html>
	</xsl:template>


	<!-- Template for ProfileBody representation -->
	<xsl:template match="gsdml:ProfileBody">
		<!-- overview -->
		<table width="100%">
			<tr>
				<th colspan="2" style="font-size:14pt">
					<xsl:value-of select="gsdml:DeviceFunction/gsdml:Family/@ProductFamily"/>
				</th>
			</tr>
			<tr>
				<td>Vendor</td>
				<td><xsl:value-of select="gsdml:DeviceIdentity/gsdml:VendorName/@Value"/></td>
			</tr>
			<tr>
				<td width="20%">Vendor ID</td>
				<td><xsl:value-of select="gsdml:DeviceIdentity/@VendorID"/></td>
			</tr>
			<tr>
				<td width="20%">Main family</td>
				<td><xsl:value-of select="gsdml:DeviceFunction/gsdml:Family/@MainFamily"/></td>
			</tr>
			<tr>
				<td width="20%">Product family</td>
				<td><xsl:value-of select="gsdml:DeviceFunction/gsdml:Family/@ProductFamily"/></td>
			</tr>
			<tr>
				<td>Device ID</td>
				<td><xsl:value-of select="gsdml:DeviceIdentity/@DeviceID"/></td>
			</tr>
			<tr>
				<td>Information</td>
				<td>
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="gsdml:DeviceIdentity/gsdml:InfoText/@TextId"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<th colspan="2">Content</th>
			</tr>
			<tr>
				<td colspan="2"><a href="#DAPLIST">Device Access Points</a></td>
			</tr>
			<xsl:if test="gsdml:ApplicationProcess/gsdml:ModuleList">
				<tr>
					<td colspan="2"><a href="#MODULELIST">Modules</a></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:ApplicationProcess/gsdml:SubmoduleList">
				<tr>
					<td colspan="2"><a href="#SUBMODULELIST">Submodules</a></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:ApplicationProcess/gsdml:ValueList">
				<tr>
					<td colspan="2"><a href="#PARAMLIST">Parameter of Modules</a></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:ApplicationProcess/gsdml:ChannelDiagList">
				<tr>
					<td colspan="2"><a href="#CHANNELDIAGLIST">Channel Errors</a></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:ApplicationProcess/gsdml:UnitDiagTypeList">
				<tr>
					<td colspan="2"><a href="#UNITDIAGLIST">Manufacturer Specific Diagnosis</a></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:ApplicationProcess/gsdml:LogBookEntryList">
				<tr>
					<td colspan="2"><a href="#LOGBOOKENTRYLIST">Logbook Entries</a></td>
				</tr>
			</xsl:if>
			<!-- GraphicsList, CategoryList and ExternalTextList are not shown. Instead, each object that references an element of these lists will show the referenced element. -->
		</table>
		<!-- contents -->
		<h2><a name="DAPLIST">Device Access Points</a></h2>
		<xsl:apply-templates select="gsdml:ApplicationProcess/gsdml:DeviceAccessPointList"/>
		<xsl:if test="gsdml:ApplicationProcess/gsdml:ModuleList">
			<h2><a name="MODULELIST">Modules</a></h2>
			<xsl:apply-templates select="gsdml:ApplicationProcess/gsdml:ModuleList"/>
		</xsl:if>
		<xsl:if test="gsdml:ApplicationProcess/gsdml:SubmoduleList">
			<h2><a name="SUBMODULELIST">Submodules</a></h2>
			<xsl:apply-templates select="gsdml:ApplicationProcess/gsdml:SubmoduleList"/>
		</xsl:if>
		<xsl:if test="gsdml:ApplicationProcess/gsdml:ValueList">
			<h2><a name="PARAMLIST">Parameter of Modules</a></h2>
			<xsl:apply-templates select="gsdml:ApplicationProcess/gsdml:ValueList"/>
		</xsl:if>
		<xsl:if test="gsdml:ApplicationProcess/gsdml:ChannelDiagList">
			<h2><a name="CHANNELDIAGLIST">Channel Errors</a></h2>
			<xsl:apply-templates select="gsdml:ApplicationProcess/gsdml:ChannelDiagList"/>
		</xsl:if>
		<xsl:if test="gsdml:ApplicationProcess/gsdml:UnitDiagTypeList">
			<h2><a name="UNITDIAGLIST">Manufacturer Specific Diagnosis</a></h2>
			<xsl:apply-templates select="gsdml:ApplicationProcess/gsdml:UnitDiagTypeList"/>
		</xsl:if>
		<xsl:if test="gsdml:ApplicationProcess/gsdml:LogBookEntryList">
			<h2><a name="LOGBOOKENTRYLIST">Logbook Entries</a></h2>
			<xsl:apply-templates select="gsdml:ApplicationProcess/gsdml:LogBookEntryList"/>
		</xsl:if>
	</xsl:template>


	<!-- Template for DeviceAccessPointItem (DAP) representation -->
	<xsl:template match="gsdml:DeviceAccessPointList/gsdml:DeviceAccessPointItem">
		<table width="100%">
			<colgroup>
				<col width="20%"/>
				<col span="8" width="10%"/>
			</colgroup>
			<tr>
				<th colspan="9">Device Access Point ID=<xsl:value-of select="@ID"/>:
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="gsdml:ModuleInfo/gsdml:Name/@TextId"/>
					</xsl:call-template>
				</th>
			</tr>
			<tr>
				<td>Module Ident Number</td>
				<td colspan="8"><xsl:value-of select="@ModuleIdentNumber"/></td>
			</tr>
			<tr>
				<td>PNIO Version</td>
				<td colspan="8"><xsl:value-of select="@PNIO_Version"/></td>
			</tr>
			<xsl:call-template name="DisplayModuleInfo"/>
			<xsl:if test="gsdml:CertificationInfo">
				<xsl:apply-templates select="gsdml:CertificationInfo"/>
			</xsl:if>
			<tr>
				<td>Maximum Input Length</td>
				<td colspan="8"><xsl:value-of select="gsdml:IOConfigData/@MaxInputLength"/> Bytes</td>
			</tr>
			<tr>
				<td>Maximum Output Length</td>
				<td colspan="8"><xsl:value-of select="gsdml:IOConfigData/@MaxOutputLength"/> Bytes</td>
			</tr>
			<xsl:if test="gsdml:IOConfigData/@MaxDataLength">
				<tr>
					<td>Maximum Data Length</td>
					<td colspan="8"><xsl:value-of select="gsdml:IOConfigData/@MaxDataLength"/></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:IOConfigData/@MaxApplicationInputLength">
				<tr>
					<td>Maximum Application Input Length</td>
					<td colspan="8"><xsl:value-of select="gsdml:IOConfigData/@MaxApplicationInputLength"/></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:IOConfigData/@MaxApplicationOutputLength">
				<tr>
					<td>Maximum Application Output Length</td>
					<td colspan="8"><xsl:value-of select="gsdml:IOConfigData/@MaxApplicationOutputLength"/></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:IOConfigData/@MaxApplicationDataLength">
				<tr>
					<td>Maximum Application Data Length</td>
					<td colspan="8"><xsl:value-of select="gsdml:IOConfigData/@MaxApplicationDataLength"/></td>
				</tr>
			</xsl:if>
			<tr>
				<td>Physical Slots</td>
				<td colspan="8"><xsl:value-of select="@PhysicalSlots"/></td>
			</tr>
			<tr>
				<td>Minimum Device Interval</td>
				<td colspan="8"><xsl:value-of select="@MinDeviceInterval*0.03125"/> ms</td>
			</tr>
			<xsl:if test="@ImplementationType">
				<tr>
					<td>Based on</td>
					<td colspan="8"><xsl:value-of select="@ImplementationType"/></td>
				</tr>
			</xsl:if>
			<tr>
				<td>DNS Compliant Name</td>
				<td colspan="8"><xsl:value-of select="@DNS_CompatibleName"/></td>
			</tr>
			<tr>
				<td>Supports Extended Assigment of IP Address</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@ExtendedAddressAssignmentSupported"/>
					</xsl:call-template>
				</td>
			</tr>
			<xsl:if test="@AllowedInSlots">
				<tr>
					<td>Allowed in Slots</td>
					<td colspan="8"><xsl:value-of select="@AllowedInSlots"/></td>
				</tr>
			</xsl:if>
			<tr>
				<td>Fixed in Slots</td>
				<td colspan="8"><xsl:value-of select="@FixedInSlots"/></td>
			</tr>
			<tr>
				<td>Instance Field of the Object UUID</td>
				<td colspan="8"><xsl:value-of select="@ObjectUUID_LocalIndex"/></td>
			</tr>
			<tr>
				<td>Supports Multiple Write</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@MultipleWriteSupported"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Requires IOPS/IOCS</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@IOXS_Required"/>
						<xsl:with-param name="BDefault" select="'Yes'"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>IP Address Assignment Methods</td>
				<xsl:choose>
					<xsl:when test="@AddressAssignment">
						<td colspan="8"><xsl:value-of select="@AddressAssignment"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">DCP</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Remote Application Timeout</td>
				<xsl:choose>
					<xsl:when test="@RemoteApplicationTimeout">
						<td colspan="8"><xsl:value-of select="@RemoteApplicationTimeout"/> s</td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">300 s</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Maximum Supported Record Size</td>
				<xsl:choose>
					<xsl:when test="@MaxSupportedRecordSize">
						<td colspan="8"><xsl:value-of select="@MaxSupportedRecordSize"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">4068</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Power on to Communication Ready</td>
				<xsl:choose>
					<xsl:when test="@PowerOnToCommReady">
						<td colspan="8"><xsl:value-of select="@PowerOnToCommReady"/> ms</td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">0 ms</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Parameterization Speedup Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@ParameterizationSpeedupSupported"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Name of Station not Transferable</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@NameOfStationNotTransferable"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Shared Device Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@SharedDeviceSupported"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Shared Input Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@SharedInputSupported"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Device Access Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@DeviceAccessSupported"/>
					</xsl:call-template>
				</td>
			</tr>
			<xsl:if test="@WebServer">
				<tr>
					<td>Web Server partial URL</td>
					<td colspan="8"><xsl:value-of select="@WebServer"/></td>
				</tr>
			</xsl:if>
			<xsl:if test="@AutoConfigurationSupported">
				<tr>
					<td>Auto Configuration Supported</td>
					<td>
						<xsl:call-template name="PrintBoolean">
							<xsl:with-param name="Value" select="@AutoConfigurationSupported"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:if>
			<xsl:if test="@CIR_Supported">
				<tr>
					<td>CiR Supported</td>
					<td>
						<xsl:call-template name="PrintBoolean">
							<xsl:with-param name="Value" select="@CIR_Supported"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:if>
			<xsl:if test="@PrmBeginPrmEndSequenceSupported">
				<tr>
					<td>PrmBeginPrmEndSequenceSupported</td>
					<td>
						<xsl:call-template name="PrintBoolean">
							<xsl:with-param name="Value" select="@PrmBeginPrmEndSequenceSupported"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:if>
			<xsl:if test="@LLDP_NoD_Supported">
				<tr>
					<td>LLDP_NoD_Supported</td>
					<td>
						<xsl:call-template name="PrintBoolean">
							<xsl:with-param name="Value" select="@LLDP_NoD_Supported"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:if>
			<xsl:if test="@ResetToFactoryModes">
				<tr>
					<td>Reset to Factory Modes</td>
					<td>
						<xsl:value-of select="@ResetToFactoryModes"/>
					</td>
				</tr>
			</xsl:if>
			<xsl:call-template name="DisplayAbstractModule"/>
			<xsl:if test="gsdml:SystemRedundancy">
				<tr>
					<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">System Redundancy</td>
				</tr>
				<tr>
					<td>Device Type</td>
					<td colspan="8"><xsl:value-of select="gsdml:SystemRedundancy/@DeviceType"/></td>
				</tr>
				<tr>
					<td>Max Switch Over Time</td>
					<td colspan="8"><xsl:value-of select="gsdml:SystemRedundancy/@MaxSwitchOverTime"/> ms</td>
				</tr>
				<xsl:if test="gsdml:SystemRedundancy/@PrimaryAR_OnBothNAPsSupported">
					<tr>
						<td>Primary AR On Both NAPs Supported</td>
						<td colspan="8">
							<xsl:call-template name="PrintBoolean">
								<xsl:with-param name="Value" select="gsdml:SystemRedundancy/@PrimaryAR_OnBothNAPsSupported"/>
							</xsl:call-template>
						</td>
					</tr>
				</xsl:if>
				<tr>
					<td>Real Time Input On Backup AR Supported</td>
					<td colspan="8">
						<xsl:choose>
							<xsl:when test="gsdml:SystemRedundancy/@RT_InputOnBackupAR_Supported">
								<xsl:call-template name="PrintBoolean">
									<xsl:with-param name="Value" select="gsdml:SystemRedundancy/@RT_InputOnBackupAR_Supported"/>
								</xsl:call-template>
							</xsl:when>
							<xsl:otherwise>
								No
							</xsl:otherwise>
						</xsl:choose>
					</td>
				</tr>
				<tr>
					<td>Number Of AR Sets</td>
					<td colspan="8">
						<xsl:choose>
							<xsl:when test="gsdml:SystemRedundancy/@NumberOfAR_Sets">
								<xsl:value-of select="gsdml:SystemRedundancy/@NumberOfAR_Sets"/>
							</xsl:when>
							<xsl:otherwise>
								1
							</xsl:otherwise>
						</xsl:choose>
					</td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:UseableModules/gsdml:ModuleItemRef">
				<tr>
					<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Useable Modules</td>
				</tr>
				<tr>
					<td style="font-weight:bold">Module ID</td>
					<td style="font-weight:bold">Name</td>
					<td colspan="4" style="font-weight:bold">Information</td>
					<td style="font-weight:bold">Allowed in Slots</td>
					<td style="font-weight:bold">Used in Slots</td>
					<td style="font-weight:bold">Fixed in Slots</td>
				</tr>
			</xsl:if>
			<xsl:for-each select="gsdml:UseableModules/gsdml:ModuleItemRef">
				<tr>
					<td>
							<xsl:variable name="MIT" select="@ModuleItemTarget"/>
							<a href="#{generate-id(/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ModuleList/gsdml:ModuleItem[@ID=$MIT])}">
								<xsl:value-of select="@ModuleItemTarget"/>
							</a>
					</td>
					<td>
						<xsl:call-template name="GetModuleName">
							<xsl:with-param name="ModuleItemTarget" select="@ModuleItemTarget"/>
						</xsl:call-template>
					</td>
					<td colspan="4">
						<xsl:call-template name="GetModuleInformation">
							<xsl:with-param name="ModuleItemTarget" select="@ModuleItemTarget"/>
						</xsl:call-template>
					</td>
					<td><xsl:value-of select="@AllowedInSlots"/></td>
					<td><xsl:value-of select="@UsedInSlots"/></td>
					<td><xsl:value-of select="@FixedInSlots"/></td>
				</tr>
			</xsl:for-each> <!-- UseableModules/ModuleItemRef -->
			<!-- ApplicationRelations are obsolete here (have been moved to SystemDefinedSubmoduleList/InterfaceSubmoduleItem) -->
			<xsl:if test="gsdml:SlotList/gsdml:SlotItem">
				<tr>
					<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Slots</td>
				</tr>
				<tr>
					<td style="font-weight:bold">Number</td>
					<td colspan="8" style="font-weight:bold">Function of the slot</td>
				</tr>
			</xsl:if>
			<xsl:for-each select="gsdml:SlotList/gsdml:SlotItem">
				<tr>
					<td><xsl:value-of select="@SlotNumber"/></td>
					<td colspan="8">
						<xsl:call-template name="GetText">
							<xsl:with-param name="TextId" select="@TextId"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:for-each> <!-- SlotList/SlotItem -->
			<xsl:if test="gsdml:SlotGroups/gsdml:SlotGroup">
				<tr>
					<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Slot Groups</td>
				</tr>
				<tr>
					<td colspan="2" style="font-weight:bold">Name</td>
					<td colspan="3" style="font-weight:bold">SlotList</td>
					<td colspan="4" style="font-weight:bold">Information</td>
				</tr>
			</xsl:if>
			<xsl:for-each select="gsdml:SlotGroups/gsdml:SlotGroup">
				<tr>
					<td colspan="2">
						<xsl:call-template name="GetText">
							<xsl:with-param name="TextId" select="gsdml:Name/@TextId"/>
						</xsl:call-template>
					</td>
					<td colspan="3"><xsl:value-of select="@SlotList"/></td>
					<td colspan="4">
						<xsl:call-template name="GetText">
							<xsl:with-param name="TextId" select="gsdml:InfoText/@TextId"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:for-each> <!-- SlotGroups/SlotGroup -->
			<xsl:for-each select="gsdml:ARVendorBlock/gsdml:Request">
				<tr>
					<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">
						ARVendorBlock Length=<xsl:value-of select="@Length"/>
					</td>
				</tr>
				<xsl:call-template name="DisplayRecordDataBase"/>
			</xsl:for-each> <!-- ARVendorBlock/Request -->
		</table>
	</xsl:template>


	<!-- Template for ModuleItem representation -->
	<xsl:template match="gsdml:ModuleList/gsdml:ModuleItem">
		<a name="{generate-id(.)}"/>
		<table width="100%">
			<colgroup>
				<col width="20%"/>
				<col span="8" width="10%"/>
			</colgroup>
			<tr>
				<th colspan="9">Module ID=<xsl:value-of select="@ID"/>:
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="gsdml:ModuleInfo/gsdml:Name/@TextId"/>
					</xsl:call-template>
				</th>
			</tr>
			<tr>
				<td>Module Ident Number</td>
				<td colspan="8"><xsl:value-of select="@ModuleIdentNumber"/></td>
			</tr>
			<xsl:call-template name="DisplayModuleInfo"/>
			<xsl:if test="@FieldbusType">
				<tr>
					<td>Fieldbus Type</td>
					<td colspan="8"><xsl:value-of select="@FieldbusType"/>: <xsl:call-template name="PrintFieldbusType"><xsl:with-param name="Value" select="@FieldbusType"/></xsl:call-template></td>
				</tr>
			</xsl:if>
			<xsl:call-template name="DisplayAbstractModule"/>
		</table>
	</xsl:template>


	<!-- Template for Submodule representation -->
	<xsl:template match="gsdml:SubmoduleList/gsdml:SubmoduleItem">
		<a name="{generate-id(.)}"/>
		<table width="100%">
			<colgroup>
				<col width="20%"/>
				<col span="8" width="10%"/>
			</colgroup>
			<tr>
				<th colspan="9">Submodule ID=<xsl:value-of select="@ID"/>:
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="gsdml:ModuleInfo/gsdml:Name/@TextId"/>
					</xsl:call-template>
				</th>
			</tr>
			<tr>
				<td>Submodule Ident Number</td>
				<td colspan="8"><xsl:value-of select="@SubmoduleIdentNumber"/></td>
			</tr>
			<xsl:call-template name="DisplayModuleInfo"/>
			<xsl:if test="@RequiredSchemaVersion">
				<tr>
					<td>Requires engineering tool which supports at least GSDML version</td>
					<td colspan="8"><xsl:value-of select="@RequiredSchemaVersion"/></td>
				</tr>
			</xsl:if>
			<xsl:call-template name="DisplayAbstractSubmodule"/>
		</table>
	</xsl:template>


	<!-- Template for PortSubmodule representation -->
	<xsl:template match="gsdml:SubmoduleList/gsdml:PortSubmoduleItem">
		<a name="{generate-id(.)}"/>
		<table width="100%">
			<colgroup>
				<col width="20%"/>
				<col span="8" width="10%"/>
			</colgroup>
			<tr>
				<th colspan="9">Port ID=<xsl:value-of select="@ID"/>:
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="gsdml:ModuleInfo/gsdml:Name/@TextId"/>
					</xsl:call-template>
				</th>
			</tr>
			<tr>
				<td>Submodule Ident Number</td>
				<td colspan="8"><xsl:value-of select="@SubmoduleIdentNumber"/></td>
			</tr>
			<xsl:call-template name="DisplayModuleInfo"/>
			<!-- Graphics -->
			<xsl:if test="@RequiredSchemaVersion">
				<tr>
					<td>Requires engineering tool which supports at least GSDML version</td>
					<td colspan="8"><xsl:value-of select="@RequiredSchemaVersion"/></td>
				</tr>
			</xsl:if>
			<xsl:if test="@ShortPreamble100MBitSupported">
			<tr>
				<td>Short Preamble 100MBit Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@ShortPreamble100MBitSupported"/>
					</xsl:call-template>
				</td>
			</tr>
			</xsl:if>
			<xsl:call-template name="DisplayAbstractPortSubmodule"/>
		</table>
	</xsl:template>


	<!-- Template for ValueList representation -->
	<xsl:template match="gsdml:ValueList/gsdml:ValueItem">
		<table width="100%">
			<tr>
				<th colspan="2" style="font-weight:bold;background-color:#294D96; color:white"><a name="{generate-id(.)}"/>Parameter ID=<xsl:value-of select="@ID"/></th>
			</tr>
			<xsl:if test="gsdml:Help">
				<tr>
					<td width="20%">Help Text</td>
					<td>
						<xsl:call-template name="GetText">
							<xsl:with-param name="TextId" select="gsdml:Help/@TextId"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:Assignments">
				<tr>
					<td width="20%" style="font-weight:bold">Value</td>
					<td style="font-weight:bold">Content</td>
				</tr>
			</xsl:if>
			<xsl:for-each select="gsdml:Assignments/gsdml:Assign">
				<tr>
					<td><xsl:value-of select="@Content"/></td>
					<td>
						<xsl:call-template name="GetText">
							<xsl:with-param name="TextId" select="@TextId"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:for-each> <!-- Assignments/Assign -->
		</table>
	</xsl:template>


	<!-- Template for ChannelDiagList representation -->
	<!-- Special: only one list for all (Profile)ChannelDiagItems -->
	<xsl:template match="gsdml:ChannelDiagList">
		<table width="100%">
			<tr>
				<th colspan="6">Channel Errors</th>
			</tr>
			<tr>
				<td width="10%" style="font-weight:bold">Error Type</td>
				<td width="10%" style="font-weight:bold">Extended Error Type</td>
				<td width="10%" style="font-weight:bold">Maintenance</td>
				<td width="10%" style="font-weight:bold">API</td>
				<td style="font-weight:bold">Error Text</td>
				<td style="font-weight:bold">Help Text</td>
			</tr>
			<xsl:for-each select="gsdml:ChannelDiagItem">
				<xsl:call-template name="DisplayChannelDiagItem"/>
			</xsl:for-each>
			<xsl:for-each select="gsdml:ProfileChannelDiagItem">
				<xsl:call-template name="DisplayChannelDiagItem"/>
			</xsl:for-each>
		</table>
	</xsl:template>


	<!-- Template for (Profile)UnitDiagTypeItem representation -->
	<xsl:template match="gsdml:UnitDiagTypeList/gsdml:UnitDiagTypeItem|gsdml:UnitDiagTypeList/gsdml:ProfileUnitDiagTypeItem">
		<table width="100%">
			<tr>
				<th colspan="7" style="font-weight:bold;background-color:#294D96; color:white">
					 User Structure Identifier: <xsl:value-of select="@UserStructureIdentifier"/>
				</th>
				<th colspan="2" style="font-weight:bold;background-color:#294D96; color:white">
					<xsl:if test="@API">
						API=<xsl:value-of select="@API"/>
					</xsl:if>
				</th>
			</tr>
			<xsl:call-template name="DisplayRef">
				<xsl:with-param name="Context" select="'Diag'"/>
			</xsl:call-template>
		</table>
	</xsl:template>


	<!-- Template for LogBookEntryList representation -->
	<xsl:template match="gsdml:LogBookEntryList">
		<table width="100%">
			<tr>
				<th style="font-weight:bold;background-color:#294D96; color:white" colspan="2">
					PNIOStatus
				</th>
				<th style="font-weight:bold;background-color:#294D96; color:white" colspan="4">
					Name
				</th>
				<th style="font-weight:bold;background-color:#294D96; color:white" colspan="2">
					Help
				</th>
			</tr>
			<xsl:for-each select="gsdml:LogBookEntryItem">
				<xsl:choose>
					<xsl:when test="gsdml:ErrorCode2Value">
						<tr>
							<td colspan="2">
								<xsl:call-template name="PrintHex">
									<xsl:with-param name="number" select="@Status"/>
								</xsl:call-template>nn
							</td>
							<td colspan="4">
								<xsl:call-template name="GetText">
									<xsl:with-param name="TextId" select="gsdml:ErrorCode2Value/gsdml:Name/@TextId"/>
								</xsl:call-template>
							</td>
							<td colspan="2">
								<xsl:if test="gsdml:ErrorCode2Value/gsdml:Help">
										<xsl:call-template name="GetText">
											<xsl:with-param name="TextId" select="gsdml:ErrorCode2Value/gsdml:Help/@TextId"/>
										</xsl:call-template>
								</xsl:if>
							</td>
						</tr>
					</xsl:when>
					<xsl:otherwise>
						<xsl:for-each select="gsdml:ErrorCode2List/gsdml:ErrorCode2Item">
							<tr>
								<td colspan="2">
									<xsl:call-template name="PrintHex">
										<xsl:with-param name="number" select="../../@Status"/>
									</xsl:call-template><xsl:call-template name="PrintHexOctet">
										<xsl:with-param name="number" select="@ErrorCode2"/>
									</xsl:call-template>
								</td>
								<td colspan="4">
									<xsl:call-template name="GetText">
										<xsl:with-param name="TextId" select="gsdml:Name/@TextId"/>
									</xsl:call-template>
								</td>
								<td colspan="2">
									<xsl:if test="gsdml:Help">
										<xsl:call-template name="GetText">
											<xsl:with-param name="TextId" select="gsdml:Help/@TextId"/>
										</xsl:call-template>
									</xsl:if>
								</td>
							</tr>
						</xsl:for-each>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:for-each>
		</table>
	</xsl:template>


<!-- Template for CertificationInfo representation -->
	<xsl:template match="gsdml:CertificationInfo">
		<tr>
			<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Certification Info</td>
		</tr>
		<tr>
			<td>Conformance Class</td>
			<td colspan="8">
				<xsl:value-of select="@ConformanceClass"/>
			</td>
		</tr>
		<tr>
			<td>Application Class</td>
			<td colspan="8">
				<xsl:value-of select="@ApplicationClass"/>
			</td>
		</tr>
		<xsl:if test="@NetloadClass">
			<tr>
				<td>Netload Class</td>
				<td colspan="8">
					<xsl:value-of select="@NetloadClass"/>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:ProfileList">
			<tr>
				<td colspan="9" style="font-weight:bold;">Profile Items</td>
			</tr>
			<xsl:for-each select="gsdml:ProfileList/gsdml:ProfileItem">
				<tr>
					<td colspan="9">
						Profile Name: <xsl:value-of select="@ProfileName"/>
						<xsl:if test="@Class">
						 -- Class: <xsl:value-of select="@Class"/>
						</xsl:if>
					</td>
				</tr>
			</xsl:for-each>
		</xsl:if>
	</xsl:template>

<!--============
Called templates
============-->

	<!-- Template for ModuleInfo representation -->
	<!-- Exception: Name is done outside -->
	<xsl:template name="DisplayModuleInfo">
		<tr>
			<td>Information</td>
			<td colspan="8">
				<xsl:call-template name="GetText">
					<xsl:with-param name="TextId" select="gsdml:ModuleInfo/gsdml:InfoText/@TextId"/>
				</xsl:call-template>
			</td>
		</tr>
		<xsl:if test="gsdml:ModuleInfo/gsdml:Family">
			<tr>
				<td>Family</td>
				<td colspan="4"><xsl:value-of select="gsdml:ModuleInfo/gsdml:Family/@MainFamily"/></td>
				<td colspan="4"><xsl:value-of select="gsdml:ModuleInfo/gsdml:Family/@ProductFamily"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:ModuleInfo/gsdml:VendorName">
			<tr>
				<td>Vendor Name</td>
				<td colspan="8"><xsl:value-of select="gsdml:ModuleInfo/gsdml:VendorName/@Value"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:ModuleInfo/gsdml:OrderNumber">
			<tr>
				<td>Order Number</td>
				<td colspan="8"><xsl:value-of select="gsdml:ModuleInfo/gsdml:OrderNumber/@Value"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:ModuleInfo/gsdml:HardwareRelease/@Value">
			<tr>
				<td>Hardware Version</td>
				<td colspan="8"><xsl:value-of select="gsdml:ModuleInfo/gsdml:HardwareRelease/@Value"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:ModuleInfo/gsdml:SoftwareRelease/@Value">
			<tr>
				<td>Software Version</td>
				<td colspan="8"><xsl:value-of select="gsdml:ModuleInfo/gsdml:SoftwareRelease/@Value"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:ModuleInfo/@CategoryRef">
			<tr>
				<td>Category</td>
				<td colspan="4">
					<xsl:call-template name="GetCategoryName">
						<xsl:with-param name="CategoryRef" select="gsdml:ModuleInfo/@CategoryRef"/>
					</xsl:call-template>
				</td>
				<td colspan="4">
					<xsl:call-template name="GetCategoryInformation">
						<xsl:with-param name="CategoryRef" select="gsdml:ModuleInfo/@CategoryRef"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:ModuleInfo/@SubCategory1Ref">
			<tr>
				<td>Subcategory</td>
				<td colspan="4">
					<xsl:call-template name="GetCategoryName">
						<xsl:with-param name="CategoryRef" select="gsdml:ModuleInfo/@SubCategory1Ref"/>
					</xsl:call-template>
				</td>
				<td colspan="4">
					<xsl:call-template name="GetCategoryInformation">
						<xsl:with-param name="CategoryRef" select="gsdml:ModuleInfo/@SubCategory1Ref"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
	</xsl:template>


	<!-- Template for Module representation: common parts of DeviceAccessPointItem (DAP) and ModuleItem -->
	<!-- Exception: ID and ModuleIdentNumber is common but done outside -->
	<!-- Exception: FieldbusIntegrationSlots and SystemDefinedSubmoduleList/InterfaceSubmoduleItem are also done here although only at DAP -->
	<xsl:template name="DisplayAbstractModule">
		<xsl:if test="@PhysicalSubslots">
			<tr>
				<td>Physical Subslots</td>
				<td colspan="8"><xsl:value-of select="@PhysicalSubslots"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="@RequiredSchemaVersion">
			<tr>
				<td>Requires Engineering Tool which supports at least GSDML version</td>
				<td colspan="8"><xsl:value-of select="@RequiredSchemaVersion"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:FieldbusIntegrationSlots">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:white; color:#294D96">This DAP supports Fieldbus Integration:</td>
			</tr>
			<tr>
				<td>Max. Number of Slots</td>
				<td colspan="8"><xsl:value-of select="gsdml:FieldbusIntegrationSlots/@MaxSupported"/></td>
			</tr>
			<tr>
				<td>Range for Slotnumbers</td>
				<xsl:choose>
					<xsl:when test="gsdml:FieldbusIntegrationSlots/@Range">
						<td colspan="8"><xsl:value-of select="gsdml:FieldbusIntegrationSlots/@Range"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">
							<xsl:call-template name="FirstAvailableSlot">
								<xsl:with-param name="ValueList" select="@PhysicalSlots"/>
							</xsl:call-template>..32767</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:SubslotList">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Subslots of Module</td>
			</tr>
			<tr>
				<td style="font-weight:bold">Subslot Number</td>
				<td colspan="8" style="font-weight:bold">Subslot Label</td>
			</tr>
			<xsl:for-each select="gsdml:SubslotList/gsdml:SubslotItem">
				<tr>
					<td>
						<xsl:value-of select="@SubslotNumber"/>
						(<xsl:call-template name="PrintHex"><xsl:with-param name="number" select="@SubslotNumber"/></xsl:call-template>)
					</td>
					<td colspan="8">
							<xsl:call-template name="GetText">
								<xsl:with-param name="TextId" select="@TextId"/>
							</xsl:call-template>
					</td>
				</tr>
			</xsl:for-each> <!-- SubslotList/SubslotItem -->
		</xsl:if>
		<xsl:for-each select="gsdml:VirtualSubmoduleList/gsdml:VirtualSubmoduleItem">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Submodule ID=<xsl:value-of select="@ID"/>:
						<xsl:call-template name="GetText">
							<xsl:with-param name="TextId" select="gsdml:ModuleInfo/gsdml:Name/@TextId"/>
						</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Submodule Ident Number</td>
				<td colspan="8"><xsl:value-of select="@SubmoduleIdentNumber"/></td>
			</tr>
			<xsl:call-template name="DisplayModuleInfo"/>
			<xsl:if test="@FixedInSubslots">
				<tr>
					<td>Fixed in Subslots</td>
					<td colspan="8"><xsl:value-of select="@FixedInSubslots"/></td>
				</tr>
			</xsl:if>
			<xsl:call-template name="DisplayAbstractSubmodule"/>
		</xsl:for-each> <!-- VirtualSubmoduleList/VirtualSubmoduleItem-->
		<xsl:for-each select="gsdml:SystemDefinedSubmoduleList/gsdml:InterfaceSubmoduleItem">
			<xsl:call-template name="DisplayInterfaceSubmodule"/>
		</xsl:for-each> <!-- SystemDefinedSubmoduleList/InterfaceSubmoduleItem -->
		<xsl:for-each select="gsdml:SystemDefinedSubmoduleList/gsdml:PortSubmoduleItem">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Port ID=<xsl:value-of select="@ID"/>:
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="@TextId"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Submodule Ident Number</td>
				<td colspan="8"><xsl:value-of select="@SubmoduleIdentNumber"/></td>
			</tr>
			<tr>
				<td>Subslot Number</td>
				<td colspan="8">
					<xsl:value-of select="@SubslotNumber"/>
					(<xsl:call-template name="PrintHex"><xsl:with-param name="number" select="@SubslotNumber"/></xsl:call-template>)
				</td>
			</tr>
			<xsl:call-template name="DisplayAbstractPortSubmodule"/>
		</xsl:for-each> <!-- SystemDefinedSubmoduleList/PortSubmoduleItem -->
<!--
		<tr>
			<td>Graphical representation</td>
			<td colspan="8">
				<xsl:element name="img">
					<xsl:attribute name="src"><xsl:call-template name="GetBitmap"><xsl:with-param name="GraphicItemTarget" select="gsdml:Graphics/gsdml:GraphicItemRef[@Type='DeviceSymbol']/@GraphicItemTarget"/></xsl:call-template></xsl:attribute>
					<xsl:attribute name="alt">Bitmap of device</xsl:attribute>
				</xsl:element>
			</td>
		</tr>
-->
		<xsl:if test="gsdml:UseableSubmodules/gsdml:SubmoduleItemRef">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Useable Submodules</td>
			</tr>
			<tr>
				<td style="font-weight:bold">Submodule ID</td>
				<td style="font-weight:bold">Name</td>
				<td colspan="4" style="font-weight:bold">Information</td>
				<td style="font-weight:bold">Allowed in Subslots</td>
				<td style="font-weight:bold">Used in Subslots</td>
				<td style="font-weight:bold">Fixed in Subslots</td>
			</tr>
		</xsl:if>
		<xsl:for-each select="gsdml:UseableSubmodules/gsdml:SubmoduleItemRef">
			<tr>
				<td>
						<xsl:variable name="SMIT" select="@SubmoduleItemTarget"/>
						<xsl:variable name="Target" select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:SubmoduleList/gsdml:SubmoduleItem[@ID=$SMIT]|/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:SubmoduleList/gsdml:PortSubmoduleItem[@ID=$SMIT]"/>
						<a href="#{generate-id($Target)}">
							<xsl:value-of select="@SubmoduleItemTarget"/>
						</a>
				</td>
				<td>
					<xsl:call-template name="GetSubmoduleName">
						<xsl:with-param name="SubmoduleItemTarget" select="@SubmoduleItemTarget"/>
					</xsl:call-template>
				</td>
				<td colspan="4">
					<xsl:call-template name="GetSubmoduleInformation">
						<xsl:with-param name="SubmoduleItemTarget" select="@SubmoduleItemTarget"/>
					</xsl:call-template>
				</td>
				<td><xsl:value-of select="@AllowedInSubslots"/></td>
				<td><xsl:value-of select="@UsedInSubslots"/></td>
				<td><xsl:value-of select="@FixedInSubslots"/></td>
			</tr>
		</xsl:for-each> <!-- UseableSubmodules/SubmoduleItemRef -->
	</xsl:template>


	<!-- Template for InterfaceSubmoduleItem representation -->
	<xsl:template name="DisplayInterfaceSubmodule">
		<tr>
			<td colspan="9" style="font-weight:bold;background-color:#294D96; color:white">Interface ID=<xsl:value-of select="@ID"/>:
				<xsl:call-template name="GetText">
					<xsl:with-param name="TextId" select="@TextId"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td>Submodule Ident Number</td>
			<td colspan="8"><xsl:value-of select="@SubmoduleIdentNumber"/></td>
		</tr>
		<tr>
			<td>Subslot Number</td>
			<xsl:choose>
				<xsl:when test="@SubslotNumber">
					<td colspan="8">
						<xsl:value-of select="@SubslotNumber"/>
						(<xsl:call-template name="PrintHex"><xsl:with-param name="number" select="@SubslotNumber"/></xsl:call-template>)
					</td>
				</xsl:when>
				<xsl:otherwise>
					<td colspan="8" style="color:#606060">32768 (0x8000)</td>
				</xsl:otherwise>
			</xsl:choose>
		</tr>
		<tr>
			<td>Supports Realtime Class</td>
			<xsl:choose>
				<xsl:when test="@SupportedRT_Class">
					<td colspan="8"><xsl:value-of select="@SupportedRT_Class"/></td>
				</xsl:when>
				<xsl:otherwise>
					<td colspan="8" style="color:#606060">Class1</td>
				</xsl:otherwise>
			</xsl:choose>
		</tr>
		<xsl:if test="@SupportedRT_Classes">
			<tr>
				<td>Supports Realtime Classes</td>
				<td colspan="8"><xsl:value-of select="@SupportedRT_Classes"/></td>
			</tr>
		</xsl:if>
		<tr>
			<td>Supports Isochronous Mode</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@IsochroneModeSupported"/>
				</xsl:call-template>
			</td>
		</tr>
		<xsl:if test="@IsochroneModeInRT_Classes">
			<tr>
				<td>Isochronous Mode supported in RT Classes</td>
				<td colspan="8"><xsl:value-of select="@IsochroneModeInRT_Classes"/></td>
			</tr>
		</xsl:if>
		<tr>
			<td>Supported Protocols</td>
			<td colspan="8"><xsl:value-of select="@SupportedProtocols"/></td>
		</tr>
		<tr>
			<td>Supported MIBs</td>
			<td colspan="8"><xsl:value-of select="@SupportedMibs"/></td>
		</tr>
		<xsl:if test="@NetworkComponentDiagnosisSupported">
			<tr>
				<td>Supports Network Component Diagnosis</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@NetworkComponentDiagnosisSupported"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<tr>
			<td>DCP_Hello Supported</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@DCP_HelloSupported"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td>PTP Boundary Supported</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@PTP_BoundarySupported"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td>DCP Boundary Supported</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@DCP_BoundarySupported"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td>Multicast Boundary Supported</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@MulticastBoundarySupported"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td>Parameterization Disallowed</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@ParameterizationDisallowed"/>
				</xsl:call-template>
			</td>
		</tr>
		<tr>
			<td>Delay Measurement Supported</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@DelayMeasurementSupported"/>
				</xsl:call-template>
			</td>
		</tr>
		<xsl:if test="@Writeable_IM_Records">
			<tr>
				<td>Writeable I&amp;M Records</td>
				<td colspan="8"><xsl:value-of select="@Writeable_IM_Records"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:General/gsdml:DCP_FlashOnceSignalUnit">
			<tr>
				<td>DCP Flash Once Signal Unit</td>
				<td colspan="8">
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="gsdml:General/gsdml:DCP_FlashOnceSignalUnit/@TextId"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:RT_Class3Properties">
			<tr>
				<td>Startup Mode</td>
				<xsl:choose>
					<xsl:when test="gsdml:RT_Class3Properties/@StartupMode">
						<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@StartupMode"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">Legacy</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<xsl:if test="gsdml:RT_Class3Properties/@ForwardingMode">
				<tr>
					<td>Forwarding Mode</td>
					<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@ForwardingMode"/></td>
				</tr>
			</xsl:if>
			<tr>
				<td>Maximum Bridge Delay</td>
				<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@MaxBridgeDelay"/> ns</td>
			</tr>
			<xsl:if test="gsdml:RT_Class3Properties/@MaxBridgeDelayFFW">
				<tr>
					<td>Max Bridge Delay FFW</td>
					<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@MaxBridgeDelayFFW"/> ns</td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:RT_Class3Properties/@MaxDFP_Feed">
				<tr>
					<td>Max DFP Feed</td>
					<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@MaxDFP_Feed"/> ns</td>
				</tr>
			</xsl:if>
			<tr>
				<td>Max DFP Frames</td>
				<xsl:choose>
					<xsl:when test="gsdml:RT_Class3Properties/@MaxDFP_Frames">
						<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@MaxDFP_Frames"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">0</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Align DFP Subframes</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="gsdml:RT_Class3Properties/@AlignDFP_Subframes"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Maximum Number of IR-Frame Data</td>
				<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@MaxNumberIR_FrameData"/></td>
			</tr>
			<tr>
				<td>Maximum Range IR Frame ID</td>
				<xsl:choose>
					<xsl:when test="gsdml:RT_Class3Properties/@MaxRangeIR_FrameID">
						<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@MaxRangeIR_FrameID"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">1024</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<xsl:if test="gsdml:RT_Class3Properties/@FragmentationType">
				<tr>
					<td>Fragmentation Type</td>
					<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@FragmentationType"/></td>
				</tr>
			</xsl:if>
			<tr>
				<td>MaxRedPeriodLength</td>
				<xsl:choose>
					<xsl:when test="gsdml:RT_Class3Properties/@MaxRedPeriodLength">
						<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@MaxRedPeriodLength"/> µs</td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">3875 µs</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Minimum frame send offset</td>
				<xsl:choose>
					<xsl:when test="gsdml:RT_Class3Properties/@MinFSO">
						<td colspan="8"><xsl:value-of select="gsdml:RT_Class3Properties/@MinFSO"/> ns</td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">5000 ns</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:SynchronisationMode">
			<tr>
				<td>Supported Sync Role</td>
				<xsl:choose>
					<xsl:when test="gsdml:SynchronisationMode/@SupportedRole">
						<td colspan="8"><xsl:value-of select="gsdml:SynchronisationMode/@SupportedRole"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">SyncSlave</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<xsl:if test="gsdml:SynchronisationMode/@MaxLocalJitter">
				<tr>
					<td>Maximum Local Jitter</td>
					<td colspan="8"><xsl:value-of select="gsdml:SynchronisationMode/@MaxLocalJitter"/> ns</td>
				</tr>
			</xsl:if>
			<tr>
				<td>T_PLL_MAX</td>
				<xsl:choose>
					<xsl:when test="gsdml:SynchronisationMode/@T_PLL_MAX">
						<td colspan="8"><xsl:value-of select="gsdml:SynchronisationMode/@T_PLL_MAX"/> ns</td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">1000 ns</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<xsl:if test="gsdml:SynchronisationMode/@SupportedSyncProtocols">
				<tr>
					<td>Supported Synchronisation Protocols</td>
					<td colspan="8"><xsl:value-of select="gsdml:SynchronisationMode/@SupportedSyncProtocols"/></td>
				</tr>
			</xsl:if>
			<tr>
				<td>Peer To Peer Jitter</td>
				<xsl:choose>
					<xsl:when test="gsdml:SynchronisationMode/@PeerToPeerJitter">
						<td colspan="8"><xsl:value-of select="gsdml:SynchronisationMode/@PeerToPeerJitter"/> ns</td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">
							<xsl:choose>
								<xsl:when test="gsdml:SynchronisationMode/@T_PLL_MAX">
									<xsl:value-of select="gsdml:SynchronisationMode/@T_PLL_MAX"/>
								</xsl:when>
								<xsl:otherwise>
									1000
								</xsl:otherwise>
							</xsl:choose>
							ns
						</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:ApplicationRelations">
			<tr>
			<td>Startup Mode</td>
				<xsl:choose>
					<xsl:when test="gsdml:ApplicationRelations/@StartupMode">
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/@StartupMode"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">Legacy</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Number of Additional Input CRs</td>
				<xsl:choose>
					<xsl:when test="gsdml:ApplicationRelations/@NumberOfAdditionalInputCR">
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/@NumberOfAdditionalInputCR"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">0</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Number of Additional Output CRs</td>
				<xsl:choose>
					<xsl:when test="gsdml:ApplicationRelations/@NumberOfAdditionalOutputCR">
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/@NumberOfAdditionalOutputCR"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">0</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Number of Additional Multicast Provider CRs</td>
				<xsl:choose>
					<xsl:when test="gsdml:ApplicationRelations/@NumberOfAdditionalMulticastProviderCR">
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/@NumberOfAdditionalMulticastProviderCR"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">0</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Number of Multicast Consumer CRs</td>
				<xsl:choose>
					<xsl:when test="gsdml:ApplicationRelations/@NumberOfMulticastConsumerCR">
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/@NumberOfMulticastConsumerCR"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">0</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>Pull Module Alarm Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="gsdml:ApplicationRelations/@PullModuleAlarmSupported"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Number of Application Relationships</td>
				<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/@NumberOfAR"/></td>
			</tr>
			<tr>
				<td>Supported Sendclock Factors<br/>(Base 31.25 µs)</td>
				<xsl:choose>
					<xsl:when test="gsdml:ApplicationRelations/gsdml:TimingProperties/@SendClock">
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:TimingProperties/@SendClock"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">32</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<xsl:if test="gsdml:ApplicationRelations/gsdml:TimingProperties/@PreferredSendClock">
				<tr>
					<td>Preferred Sendclock</td>
					<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:TimingProperties/@PreferredSendClock"/></td>
				</tr>
			</xsl:if>
			<xsl:choose>
				<xsl:when test="gsdml:ApplicationRelations/gsdml:TimingProperties/@ReductionRatio">
					<tr>
						<td>Supported Reduction Ratios</td>
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:TimingProperties/@ReductionRatio"/></td>
					</tr>
				</xsl:when>
				<xsl:when test="not(gsdml:ApplicationRelations/gsdml:TimingProperties/@ReductionRatioPow2) and not(gsdml:ApplicationRelations/gsdml:TimingProperties/@ReductionRatioNonPow2)">
					<tr>
						<td>Supported Reduction Ratios</td>
						<td colspan="8" style="color:#606060">1 2 4 8 16 32 64 128 256 512</td>
					</tr>
				</xsl:when>
			</xsl:choose>
			<xsl:if test="gsdml:ApplicationRelations/gsdml:TimingProperties/@ReductionRatioPow2">
				<tr>
					<td>Supp. Reduction Ratios for SendClocks power of 2</td>
					<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:TimingProperties/@ReductionRatioPow2"/></td>
				</tr>
			</xsl:if>
			<xsl:if test="gsdml:ApplicationRelations/gsdml:TimingProperties/@ReductionRatioNonPow2">
				<tr>
					<td>Supp. Reduction Ratios for SendClocks non power of 2</td>
					<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:TimingProperties/@ReductionRatioNonPow2"/></td>
				</tr>
			</xsl:if>
			<xsl:if test="@SupportedRT_Class='Class3' or contains(@SupportedRT_Classes, 'RT_CLASS_3')">
				<tr>
					<td>Supported RT Class 3 Sendclock Factors<br/>(Base 31.25 µs)</td>
					<xsl:choose>
						<xsl:when test="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@SendClock">
							<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@SendClock"/></td>
						</xsl:when>
						<xsl:otherwise>
							<td colspan="8" style="color:#606060">32</td>
						</xsl:otherwise>
					</xsl:choose>
				</tr>
				<xsl:if test="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@PreferredSendClock">
					<tr>
						<td>Preferred Sendclock</td>
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@PreferredSendClock"/></td>
					</tr>
				</xsl:if>
				<xsl:choose>
					<xsl:when test="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@ReductionRatio">
						<tr>
							<td>Supported RT Class 3 Reduction Ratios</td>
							<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@ReductionRatio"/></td>
						</tr>
					</xsl:when>
					<xsl:when test="not(gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@ReductionRatioPow2) and not(gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@ReductionRatioNonPow2)">
						<tr>
							<td>Supported RT Class 3 Reduction Ratios</td>
							<td colspan="8" style="color:#606060">1 2 4 8 16</td>
						</tr>
					</xsl:when>
				</xsl:choose>
				<xsl:if test="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@ReductionRatioPow2">
					<tr>
						<td>Supp. RT Class 3 Red. Ratios for SendClocks power of 2</td>
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@ReductionRatioPow2"/></td>
					</tr>
				</xsl:if>
				<xsl:if test="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@ReductionRatioNonPow2">
					<tr>
						<td>Supp. RT Class 3 Red. Ratios for SendClocks non power of 2</td>
						<td colspan="8"><xsl:value-of select="gsdml:ApplicationRelations/gsdml:RT_Class3TimingProperties/@ReductionRatioNonPow2"/></td>
					</tr>
				</xsl:if>
			</xsl:if>
		</xsl:if>
		<xsl:if test="gsdml:MediaRedundancy">
			<tr>
				<td>Supported Role</td>
				<xsl:choose>
					<xsl:when test="gsdml:MediaRedundancy/@SupportedRole">
						<td colspan="8"><xsl:value-of select="gsdml:MediaRedundancy/@SupportedRole"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="color:#606060">Client</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td>MRPD Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="gsdml:MediaRedundancy/@MRPD_Supported"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>MRT Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="gsdml:MediaRedundancy/@MRT_Supported"/>
					</xsl:call-template>
				</td>
			</tr>
			<tr>
				<td>Additional Protocols Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="gsdml:MediaRedundancy/@AdditionalProtocolsSupported"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<xsl:call-template name="DisplayRecordDataList"/>
	</xsl:template>


	<!-- Template for PortSubmodule representation: common parts of DeviceAccessPointList/DeviceAccessPointItem/SystemDefinedSubmoduleList/PortSubmoduleItem, ModuleList/ModuleItem/SystemdefinedSubmoduleList/PortSubmoduleItem and SubmoduleList/PortSubmoduleItem -->
	<!-- Exception: ID and SubmoduleIdentNumber is common but done outside -->
	<xsl:template name="DisplayAbstractPortSubmodule">
		<tr>
			<td>MAU Type</td>
			<xsl:choose>
				<xsl:when test="@MAUType">
					<td colspan="8"><xsl:value-of select="@MAUType"/></td>
				</xsl:when>
				<xsl:otherwise>
					<td colspan="8" style="color:#606060">100BASETXFD</td>
				</xsl:otherwise>
			</xsl:choose>
		</tr>
		<xsl:if test="@MAUTypes">
			<tr>
				<td>MAU Types</td>
				<td colspan="8">
					<xsl:value-of select="@MAUTypes"/>: 
					<xsl:call-template name="PrintMAUTypes">
						<xsl:with-param name="ValueList" select="@MAUTypes"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="@FiberOpticTypes">
			<tr>
				<td>Fiber Optic Types</td>
				<td colspan="8"><xsl:value-of select="@FiberOpticTypes"/>: <xsl:call-template name="PrintFOTypes"><xsl:with-param name="ValueList" select="@FiberOpticTypes"/></xsl:call-template></td>
			</tr>
		</xsl:if>
		<xsl:if test="@MaxPortTxDelay">
			<tr>
				<td>MaxPortTxDelay</td>
				<td colspan="8"><xsl:value-of select="@MaxPortTxDelay"/> ns</td>
			</tr>
		</xsl:if>
		<xsl:if test="@MaxPortRxDelay">
			<tr>
				<td>MaxPortRxDelay</td>
				<td colspan="8"><xsl:value-of select="@MaxPortRxDelay"/> ns</td>
			</tr>
		</xsl:if>
		<xsl:if test="@PortDeactivationSupported">
			<tr>
				<td>Port Deactivation Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@PortDeactivationSupported"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="@LinkStateDiagnosisCapability">
			<tr>
				<td>Link State Diagnosis Capability</td>
				<td colspan="8"><xsl:value-of select="@LinkStateDiagnosisCapability"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="@PowerBudgetControlSupported">
			<tr>
				<td>Power Budget Control Supported</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@PowerBudgetControlSupported"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="@SupportsRingportConfig">
			<tr>
				<td>Supports Ringport Config</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@SupportsRingportConfig"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="@IsDefaultRingport">
			<tr>
				<td>Is Default Ringport</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@IsDefaultRingport"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<tr>
			<td>Parameterization Disallowed</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@ParameterizationDisallowed"/>
				</xsl:call-template>
			</td>
		</tr>
		<xsl:if test="@Writeable_IM_Records">
			<tr>
				<td>Writeable I&amp;M Records</td>
				<td colspan="8"><xsl:value-of select="@Writeable_IM_Records"/></td>
			</tr>
		</xsl:if>
		<tr>
			<td>CheckMAUType Supported</td>
			<td colspan="8">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@CheckMAUTypeSupported"/>
				</xsl:call-template>
			</td>
		</tr>
		<xsl:call-template name="DisplayRecordDataList"/>
	</xsl:template>


	<!-- Template for Submodule representation: common parts of DeviceAccessPointList/DeviceAccessPointItem/VirtualSubmoduleList/VirtualSubmoduleItem, ModuleList/ModuleItem/VirtualSubmoduleList/VirtualSubmoduleItem and SubmoduleList/SubmoduleItem -->
	<!-- Exception: ID and SubmoduleIdentNumber is common but done outside -->
	<xsl:template name="DisplayAbstractSubmodule">
		<xsl:if test="@API">
			<tr>
				<td>Assigned to API</td>
				<td colspan="8"><xsl:value-of select="@API"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="@PROFIsafeSupported">
			<tr>
				<td>Supports PROFIsafe Profile</td>
				<td colspan="8">
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@PROFIsafeSupported"/>
					</xsl:call-template>
				</td>
			</tr>
		</xsl:if>
		<xsl:if test="@Writeable_IM_Records">
			<tr>
				<td>Writeable I&amp;M Records</td>
				<td colspan="8"><xsl:value-of select="@Writeable_IM_Records"/></td>
			</tr>
		</xsl:if>
		<tr>
			<td>Maximum i-Parameter Size</td>
			<xsl:choose>
				<xsl:when test="@Max_iParameterSize">
					<td colspan="8"><xsl:value-of select="@Max_iParameterSize"/></td>
				</xsl:when>
				<xsl:otherwise>
					<td colspan="8" style="color:#606060">0</td>
				</xsl:otherwise>
			</xsl:choose>
		</tr>
		<xsl:if test="@SubsysModuleDirIndex">
			<tr>
				<td>Subsys Module Dir Index</td>
				<td colspan="8"><xsl:value-of select="@SubsysModuleDirIndex"/></td>
			</tr>
		</xsl:if>
		<xsl:if test="@SupportedSubstitutionModes">
			<tr>
				<td>Supported Substitution Modes</td>
				<td colspan="8"><xsl:value-of select="@SupportedSubstitutionModes"/>: <xsl:call-template name="PrintSubstitutionModes"><xsl:with-param name="ValueList" select="@SupportedSubstitutionModes"/></xsl:call-template></td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:PROFIenergy">
			<tr>
				<td colspan="9">PROFIenergy is supported (the submodule has a SAP = Service Access Point)</td>
			</tr>
		</xsl:if>
		<xsl:if test="gsdml:SlotCluster">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:white; color:#294D96">The submodule is a Fieldbus Access Point (FAP). A Slot Cluster is associated with this submodule.</td>
			</tr>
			<tr>
				<td>Number of Slots</td>
				<td colspan="8"><xsl:value-of select="gsdml:SlotCluster/@Count"/></td>
			</tr>
			<tr>
				<td>Fieldbus Type</td>
				<td colspan="8"><xsl:value-of select="gsdml:SlotCluster/@FieldbusType"/>: <xsl:call-template name="PrintFieldbusType"><xsl:with-param name="Value" select="gsdml:SlotCluster/@FieldbusType"/></xsl:call-template></td>
			</tr>
		</xsl:if>
		<!-- IOData -->
		<xsl:if test="gsdml:IOData/@F_IO_StructureDescCRC">
			<tr>
				<td style="border-color:yellow">F_IO_StructureDescVersion</td>
				<xsl:choose>
					<xsl:when test="gsdml:IOData/@F_IO_StructureDescVersion">
						<td colspan="8" style="border-color:yellow"><xsl:value-of select="gsdml:IOData/@F_IO_StructureDescVersion"/>
						<xsl:choose>
							<xsl:when test="gsdml:IOData/@F_IO_StructureDescVersion='1'">: 16-bit CRC</xsl:when>
							<xsl:when test="gsdml:IOData/@F_IO_StructureDescVersion='2'">: 32-bit CRC</xsl:when>
						</xsl:choose>
						</td>
					</xsl:when>
					<xsl:otherwise>
						<td colspan="8" style="border-color:yellow;color:#606060">1: 16-bit CRC</td>
					</xsl:otherwise>
				</xsl:choose>
			</tr>
			<tr>
				<td style="border-color:yellow">F_IO_StructureDescCRC</td>
				<td colspan="8" style="border-color:yellow">
					<xsl:value-of select="gsdml:IOData/@F_IO_StructureDescCRC"/>
				</td>
			</tr>
		</xsl:if>
		<xsl:for-each select="gsdml:IOData/gsdml:Input">
			<xsl:call-template name="DisplayIOData">
				<xsl:with-param name="Direction" select="'In'"/>
			</xsl:call-template>
		</xsl:for-each>
		<xsl:for-each select="gsdml:IOData/gsdml:Output">
			<xsl:call-template name="DisplayIOData">
				<xsl:with-param name="Direction" select="'Out'"/>
			</xsl:call-template>
		</xsl:for-each>
		<!-- Parameter for Isochronous Mode -->
		<xsl:if test="gsdml:IsochroneMode">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:white; color:#294D96">Parameter for Isochronous Mode</td>
			</tr>
			<tr>
				<td style="font-weight:bold">Name</td>
				<td colspan="8" style="font-weight:bold">Value</td>
			</tr>
			<xsl:if test="gsdml:IsochroneMode/@IsochroneModeRequired">
				<tr>
					<td>Requires Isochronous Mode</td>
					<td colspan="8">
						<xsl:call-template name="PrintBoolean">
							<xsl:with-param name="Value" select="gsdml:IsochroneMode/@IsochroneModeRequired"/>
						</xsl:call-template>
					</td>
				</tr>
			</xsl:if>
			<tr>
				<td>T_DC_Base</td>
				<td colspan="8"><xsl:value-of select="gsdml:IsochroneMode/@T_DC_Base*0.03125"/> ms</td>
			</tr>
			<tr>
				<td>T_DC_Min</td>
				<td colspan="8"><xsl:value-of select="gsdml:IsochroneMode/@T_DC_Min*gsdml:IsochroneMode/@T_DC_Base*0.03125"/> ms</td>
			</tr>
			<tr>
				<td>T_DC_Max</td>
				<td colspan="8"><xsl:value-of select="gsdml:IsochroneMode/@T_DC_Max*gsdml:IsochroneMode/@T_DC_Base*0.03125"/> ms</td>
			</tr>
			<tr>
				<td>T_IO_Base</td>
				<td colspan="8"><xsl:value-of select="gsdml:IsochroneMode/@T_IO_Base*0.001"/> µs</td>
			</tr>
			<tr>
				<td>T_IO_InputMin</td>
				<td colspan="8"><xsl:value-of select="gsdml:IsochroneMode/@T_IO_InputMin*gsdml:IsochroneMode/@T_IO_Base*0.001"/> µs</td>
			</tr>
			<tr>
				<td>T_IO_OutputMin</td>
				<td colspan="8"><xsl:value-of select="gsdml:IsochroneMode/@T_IO_OutputMin*gsdml:IsochroneMode/@T_IO_Base*0.001"/> µs</td>
			</tr>
		</xsl:if>
		<!-- RecordDataList -->
		<xsl:call-template name="DisplayRecordDataList"/>
		<xsl:call-template name="DisplayFParameterRecordDataItem"/>
<!--
		<tr>
			<td>Graphical representation</td>
			<td colspan="8">
				<xsl:element name="img">
					<xsl:attribute name="src"><xsl:call-template name="GetBitmap"><xsl:with-param name="GraphicItemTarget" select="gsdml:Graphics/gsdml:GraphicItemRef[@Type='DeviceSymbol']/@GraphicItemTarget"/></xsl:call-template></xsl:attribute>
					<xsl:attribute name="alt">Bitmap of device</xsl:attribute>
				</xsl:element>
			</td>
		</tr>
-->
	</xsl:template>


	<!-- Template for IOData/Input and IOData/Output representation -->
	<xsl:template name="DisplayIOData">
		<xsl:param name="Direction"/>
		<tr>
			<td colspan="7" style="font-weight:bold;background-color:white; color:#294D96">Cyclic <xsl:value-of select="$Direction"/>put Data</td>
			<xsl:choose>
				<xsl:when test="@Consistency">
					<td colspan="2" style="background-color:white; color:#294D96">
						<xsl:value-of select="@Consistency"/>
					</td>
				</xsl:when>
				<xsl:otherwise>
					<td colspan="2" style="background-color:white; color:#606060">Item consistency</td>
				</xsl:otherwise>
			</xsl:choose>
		</tr>
		<tr>
			<td style="font-weight:bold">Name</td>
			<td style="font-weight:bold">Data Type</td>
			<td style="font-weight:bold">Display as Bits</td>
			<td colspan="6" style="font-weight:bold">Length [Bytes]</td>
		</tr>
		<xsl:for-each select="gsdml:DataItem">
			<tr>
				<td>
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="@TextId"/>
					</xsl:call-template>
				</td>
				<td><xsl:value-of select="@DataType"/></td>
				<td>
					<xsl:choose>
						<xsl:when test="gsdml:BitDataItem">
							<xsl:for-each select="gsdml:BitDataItem">
								Bit <xsl:value-of select="@BitOffset"/>:
								<xsl:call-template name="GetText">
									<xsl:with-param name="TextId" select="@TextId"/>
								</xsl:call-template>
								<br/>
							</xsl:for-each> <!-- BitDataItem -->
						</xsl:when>
						<xsl:otherwise>
						<xsl:if test="@UseAsBits='true'">Yes</xsl:if>
						</xsl:otherwise>
					</xsl:choose>
				</td>
				<td colspan="6"><xsl:value-of select="@Length"/></td>
			</tr>
		</xsl:for-each> <!-- DataItem -->
	</xsl:template>


	<!-- Template for RecordDataList representation -->
	<xsl:template name="DisplayRecordDataList">
		<!-- Parameter -->
		<xsl:for-each select="gsdml:RecordDataList/gsdml:ParameterRecordDataItem">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:white; color:#294D96">
					<xsl:call-template name="GetText">
						<xsl:with-param name="TextId" select="gsdml:Name/@TextId"/>
					</xsl:call-template>
					(Index: <xsl:value-of select="@Index"/> -- Length: <xsl:value-of select="@Length"/> Byte -- 
					<xsl:choose>
						<xsl:when test="@TransferSequence">
							Transfer Sequence: <xsl:value-of select="@TransferSequence"/> -- 
						</xsl:when>
						<xsl:otherwise>
							<font style="color:#606060">Transfer Sequence: 0</font> -- 
						</xsl:otherwise>
					</xsl:choose>
					<xsl:choose>
						<xsl:when test="@ChangeableWithBump">
							Changeable With Bump:
							<xsl:call-template name="PrintBoolean">
								<xsl:with-param name="Value" select="@ChangeableWithBump"/>
							</xsl:call-template>
						</xsl:when>
						<xsl:otherwise>
							<font style="color:#606060">Changeable With Bump: No</font>
						</xsl:otherwise>
					</xsl:choose>
					)
				</td>
			</tr>
			<xsl:call-template name="DisplayRecordDataBase"/>
		</xsl:for-each> <!-- RecordDataList/ParameterRecordDataItem -->
	</xsl:template>


	<!-- Template for RecordDataBaseG representation -->
	<xsl:template name="DisplayRecordDataBase">
		<xsl:if test="gsdml:Const">
			<tr>
				<td style="font-weight:bold">Byte Offset</td>
				<td colspan="8" style="font-weight:bold">Data</td>
			</tr>
		</xsl:if>
		<xsl:for-each select="gsdml:Const">
			<tr>
				<xsl:choose>
					<xsl:when test="@ByteOffset">
						<td><xsl:value-of select="@ByteOffset"/></td>
					</xsl:when>
					<xsl:otherwise>
						<td style="color:#606060">0</td>
					</xsl:otherwise>
				</xsl:choose>
				<td colspan="8"><xsl:value-of select="@Data"/></td>
			</tr>
		</xsl:for-each> <!-- Const -->
		<xsl:choose>
			<xsl:when test="gsdml:MenuList">
				<xsl:call-template name="DisplayRefHeader">
					<xsl:with-param name="Context" select="'Param'"/>
				</xsl:call-template>
				<xsl:for-each select="gsdml:MenuList/gsdml:MenuItem">
					<tr style="background-color:#cccccc;">
						<td colspan="9" style="font-weight:bold;">
							Menu ID=<xsl:value-of select="@ID"/>:
							<xsl:call-template name="GetText">
								<xsl:with-param name="TextId" select="gsdml:Name/@TextId"/>
							</xsl:call-template>
						</td>
					</tr>
					<xsl:for-each select="gsdml:ParameterRef">
						<xsl:variable name="PTarget" select="@ParameterTarget"/>
						<xsl:for-each select="../../../gsdml:Ref[@ID=$PTarget]">
							<xsl:call-template name="DisplayRefData">
								<xsl:with-param name="Context" select="'Param'"/>
							</xsl:call-template>
						</xsl:for-each>
					</xsl:for-each>
					<xsl:for-each select="gsdml:MenuRef">
						<tr>
							<td colspan="9">
								Reference to Menu ID=<xsl:value-of select="@MenuTarget"/>
							</td>
						</tr>
					</xsl:for-each>
				</xsl:for-each>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="DisplayRef">
					<xsl:with-param name="Context" select="'Param'"/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- Template for Ref Header representation -->
	<xsl:template name="DisplayRefHeader">
		<xsl:param name="Context"/> <!-- Diag or Param -->
		<xsl:if test="gsdml:Ref">
			<tr>
				<td width="20%" style="font-weight:bold;">Name</td>
				<td style="font-weight:bold;"><xsl:if test="$Context='Diag'"><xsl:attribute name="colspan">2</xsl:attribute></xsl:if>Data Type</td>
				<td style="font-weight:bold;"><xsl:if test="$Context='Diag'"><xsl:attribute name="colspan">2</xsl:attribute></xsl:if>Byte Offset</td>
				<td style="font-weight:bold;"><xsl:if test="$Context='Diag'"><xsl:attribute name="colspan">2</xsl:attribute></xsl:if>Bit Offset</td>
				<td style="font-weight:bold;"><xsl:if test="$Context='Diag'"><xsl:attribute name="colspan">2</xsl:attribute></xsl:if>Bit Length</td>
				<xsl:if test="$Context='Param'">
					<td style="font-weight:bold;">Default Value</td>
					<td style="font-weight:bold;">Allowed Values</td>
					<td style="font-weight:bold;">Changeable</td>
					<td style="font-weight:bold;">Visible</td>
				</xsl:if>
			</tr>
		</xsl:if>
	</xsl:template>


	<!-- Template for Single Ref representation -->
	<xsl:template name="DisplayRefData">
		<xsl:param name="Context"/> <!-- Diag or Param -->
		<tr>
			<td>
				<xsl:call-template name="GetText">
					<xsl:with-param name="TextId" select="@TextId"/>
				</xsl:call-template>
				<xsl:if test="@ValueItemTarget"> (Param: 
					<xsl:variable name="VIT" select="@ValueItemTarget"/>
						<a href="#{generate-id(/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ValueList/gsdml:ValueItem[@ID=$VIT])}">
							<xsl:value-of select="@ValueItemTarget"/></a>)
				</xsl:if>
			</td>
			<td><xsl:if test="$Context='Diag'"><xsl:attribute name="colspan">2</xsl:attribute></xsl:if>
				<xsl:value-of select="@DataType"/><xsl:if test="@Length">[<xsl:value-of select="@Length"/>]</xsl:if>
			</td>
			<td><xsl:if test="$Context='Diag'"><xsl:attribute name="colspan">2</xsl:attribute></xsl:if>
				<xsl:value-of select="@ByteOffset"/>
			</td>
			<td><xsl:if test="$Context='Diag'"><xsl:attribute name="colspan">2</xsl:attribute></xsl:if>
				<xsl:if test="@DataType='BitArea' or @DataType='Bit'">
				<xsl:choose>
					<xsl:when test="@BitOffset">
						<xsl:value-of select="@BitOffset"/>
					</xsl:when>
					<xsl:otherwise>
						<font style="color:#606060">0</font>
					</xsl:otherwise>
				</xsl:choose>
				</xsl:if>
			</td>
			<td><xsl:if test="$Context='Diag'"><xsl:attribute name="colspan">2</xsl:attribute></xsl:if>
				<xsl:if test="@DataType='BitArea'">
				<xsl:choose>
					<xsl:when test="@BitLength">
						<xsl:value-of select="@BitLength"/>
					</xsl:when>
					<xsl:otherwise>
						<font style="color:#606060">1</font>
					</xsl:otherwise>
				</xsl:choose>
				</xsl:if>
			</td>
			<xsl:if test="$Context='Param'">
				<td>
					<xsl:variable name="ValueId" select="@ValueItemTarget"/>
					<xsl:variable name="AssignId" select="@DefaultValue"/>
					<xsl:value-of select="@DefaultValue"/>
					<xsl:if test="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ValueList/gsdml:ValueItem[@ID=$ValueId]/gsdml:Assignments/gsdml:Assign[@Content=$AssignId]">:
						<xsl:call-template name="GetText">
							<xsl:with-param name="TextId" select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ValueList/gsdml:ValueItem[@ID=$ValueId]/gsdml:Assignments/gsdml:Assign[@Content=$AssignId]/@TextId"/>
						</xsl:call-template>
					</xsl:if>
				</td>
				<td><xsl:value-of select="@AllowedValues"/></td>
				<td>
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@Changeable"/>
						<xsl:with-param name="BDefault" select="'Yes'"/>
					</xsl:call-template>
				</td>
				<td>
					<xsl:call-template name="PrintBoolean">
						<xsl:with-param name="Value" select="@Visible"/>
						<xsl:with-param name="BDefault" select="'Yes'"/>
					</xsl:call-template>
				</td>
			</xsl:if>
		</tr>
	</xsl:template>


	<!-- Template for multiple Ref representation -->
	<xsl:template name="DisplayRef">
		<xsl:param name="Context"/> <!-- Diag or Param -->
		<xsl:call-template name="DisplayRefHeader">
			<xsl:with-param name="Context" select="$Context"/>
		</xsl:call-template>
		<xsl:for-each select="gsdml:Ref">
			<xsl:call-template name="DisplayRefData">
				<xsl:with-param name="Context" select="$Context"/>
			</xsl:call-template>
		</xsl:for-each>
	</xsl:template>

	<!-- Template for F_ParameterRecordDataItem representation -->
	<xsl:template name="DisplayFParameterRecordDataItem">
		<xsl:for-each select="gsdml:RecordDataList/gsdml:F_ParameterRecordDataItem">
			<tr>
				<td colspan="9" style="font-weight:bold;background-color:#FFFF88; color:#294D96">
					F-Parameter (Index: <xsl:value-of select="@Index"/> -- F_ParamDescCRC: <xsl:value-of select="@F_ParamDescCRC"/> -- 
					<xsl:choose>
						<xsl:when test="@TransferSequence">
							Transfer Sequence: <xsl:value-of select="@TransferSequence"/>
						</xsl:when>
						<xsl:otherwise>
							<font style="color:#606060">Transfer Sequence: 0</font> -- 
						</xsl:otherwise>
					</xsl:choose>
					<xsl:if test="@F_SupportedParameters">
						F_SupportedParameters: <xsl:value-of select="@F_SupportedParameters"/> -- 
					</xsl:if>
					<xsl:choose>
						<xsl:when test="@ChangeableWithBump">
							Changeable With Bump:
							<xsl:call-template name="PrintBoolean">
								<xsl:with-param name="Value" select="@ChangeableWithBump"/>
							</xsl:call-template>
						</xsl:when>
						<xsl:otherwise>
							<font style="color:#606060">Changeable With Bump: No</font>
						</xsl:otherwise>
					</xsl:choose>
					)
				</td>
			</tr>
			<tr>
				<td style="font-weight:bold;border-color:yellow">Name</td>
				<td style="font-weight:bold;border-color:yellow">Default Value</td>
				<td colspan="5" style="font-weight:bold;border-color:yellow">Allowed Values</td>
				<td style="font-weight:bold;border-color:yellow">Visible</td>
				<td style="font-weight:bold;border-color:yellow">Changeable</td>
			</tr>
			<xsl:for-each select="gsdml:F_Check_iPar">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_Check_iPar'"/>
					<xsl:with-param name="FDefaultValue" select="'NoCheck'"/>
					<xsl:with-param name="FAllowedValues" select="'Check NoCheck'"/>
					<xsl:with-param name="FVisible" select="'No'"/>
					<xsl:with-param name="FChangeable" select="'No'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_SIL">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_SIL'"/>
					<xsl:with-param name="FDefaultValue" select="'SIL3'"/>
					<xsl:with-param name="FAllowedValues" select="'SIL1 SIL2 SIL3 NoSIL'"/>
					<xsl:with-param name="FVisible" select="'Yes'"/>
					<xsl:with-param name="FChangeable" select="'Yes'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_CRC_Length">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_CRC_Length'"/>
					<xsl:with-param name="FDefaultValue" select="'3-Byte-CRC'"/>
					<xsl:with-param name="FAllowedValues" select="'3-Byte-CRC'"/>
					<xsl:with-param name="FVisible" select="'No'"/>
					<xsl:with-param name="FChangeable" select="'No'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_Block_ID">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_Block_ID'"/>
					<xsl:with-param name="FDefaultValue" select="'0'"/>
					<xsl:with-param name="FAllowedValues" select="'0..7'"/>
					<xsl:with-param name="FVisible" select="'Yes'"/>
					<xsl:with-param name="FChangeable" select="'No'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_Par_Version">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_Par_Version'"/>
					<xsl:with-param name="FDefaultValue" select="'1'"/>
					<xsl:with-param name="FAllowedValues" select="'1'"/>
					<xsl:with-param name="FVisible" select="'Yes'"/>
					<xsl:with-param name="FChangeable" select="'No'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_Source_Add">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_Source_Add'"/>
					<xsl:with-param name="FDefaultValue" select="'1'"/>
					<xsl:with-param name="FAllowedValues" select="'1..65534'"/>
					<xsl:with-param name="FVisible" select="'Yes'"/>
					<xsl:with-param name="FChangeable" select="'Yes'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_Dest_Add">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_Dest_Add'"/>
					<xsl:with-param name="FDefaultValue" select="'1'"/>
					<xsl:with-param name="FAllowedValues" select="'1..65534'"/>
					<xsl:with-param name="FVisible" select="'Yes'"/>
					<xsl:with-param name="FChangeable" select="'Yes'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_WD_Time">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_WD_Time'"/>
					<xsl:with-param name="FDefaultValue" select="'150'"/>
					<xsl:with-param name="FAllowedValues" select="'1..65535'"/>
					<xsl:with-param name="FVisible" select="'Yes'"/>
					<xsl:with-param name="FChangeable" select="'Yes'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_Par_CRC">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_Par_CRC'"/>
					<xsl:with-param name="FDefaultValue" select="'53356'"/>
					<xsl:with-param name="FAllowedValues" select="'0..65535'"/>
					<xsl:with-param name="FVisible" select="'Yes'"/>
					<xsl:with-param name="FChangeable" select="'Yes'"/>
				</xsl:call-template>
			</xsl:for-each>
			<xsl:for-each select="gsdml:F_iPar_CRC">
				<xsl:call-template name="DisplayFParameterLine">
					<xsl:with-param name="FParameter" select="'F_iPar_CRC'"/>
					<xsl:with-param name="FDefaultValue" select="'0'"/>
					<xsl:with-param name="FAllowedValues" select="'0..4294967295'"/>
					<xsl:with-param name="FVisible" select="'Yes'"/>
					<xsl:with-param name="FChangeable" select="'Yes'"/>
				</xsl:call-template>
			</xsl:for-each>
		</xsl:for-each>
	</xsl:template>


	<!-- Template for single F-Parameter representation -->
	<xsl:template name="DisplayFParameterLine">
		<xsl:param name="FParameter"/>
		<xsl:param name="FDefaultValue"/>
		<xsl:param name="FAllowedValues"/>
		<xsl:param name="FVisible"/>
		<xsl:param name="FChangeable"/>
		<tr>
			<td style="border-color:yellow"><xsl:value-of select="$FParameter"/></td>
			<td style="border-color:yellow">
				<xsl:choose>
					<xsl:when test="@DefaultValue">
						<xsl:call-template name="PrintFDefaultValue">
							<xsl:with-param name="Value" select="@DefaultValue"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<font style="color:#606060">
							<xsl:call-template name="PrintFDefaultValue">
								<xsl:with-param name="Value" select="$FDefaultValue"/>
							</xsl:call-template>
						</font>
					</xsl:otherwise>
				</xsl:choose>
			</td>
			<td colspan="5" style="border-color:yellow">
				<xsl:choose>
					<xsl:when test="@AllowedValues">
						<xsl:call-template name="PrintFAllowedValues">
							<xsl:with-param name="ValueList" select="@AllowedValues"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<font style="color:#606060">
							<xsl:call-template name="PrintFAllowedValues">
								<xsl:with-param name="ValueList" select="$FAllowedValues"/>
							</xsl:call-template>
						</font>
					</xsl:otherwise>
				</xsl:choose>
			</td>
			<td style="border-color:yellow">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@Visible"/>
					<xsl:with-param name="BDefault" select="$FVisible"/>
				</xsl:call-template>
			</td>
			<td style="border-color:yellow">
				<xsl:call-template name="PrintBoolean">
					<xsl:with-param name="Value" select="@Changeable"/>
					<xsl:with-param name="BDefault" select="$FChangeable"/>
				</xsl:call-template>
			</td>
		</tr>
	</xsl:template>


	<!-- Template for DefaultValue of F_Parameter -->
	<xsl:template name="PrintFDefaultValue">
		<xsl:param name="Value"/>
		<xsl:value-of select="$Value"/>
		<xsl:choose>
			<xsl:when test="local-name() = 'F_Block_ID'">
				<xsl:if test="$Value = 1"><xsl:value-of select="': uses F_iPar_CRC'"/></xsl:if>
				<xsl:if test="$Value = 2"><xsl:value-of select="': uses F_WD_Time_2'"/></xsl:if>
				<xsl:if test="$Value = 3"><xsl:value-of select="': uses F_iPar_CRC and F_WD_Time_2'"/></xsl:if>
			</xsl:when>
			<xsl:when test="local-name() = 'F_Par_Version'">
				<xsl:if test="$Value = 1"><xsl:value-of select="': V2-mode'"/></xsl:if>
			</xsl:when>
			<xsl:when test="local-name() = 'F_WD_Time'">
				<xsl:value-of select="' ms'"/>
			</xsl:when>
		</xsl:choose>
	</xsl:template>


	<!-- Template for AllowedValues of F_Parameter -->
	<xsl:template name="PrintFAllowedValues">
		<xsl:param name="ValueList"/>
		<xsl:value-of select="$ValueList"/>
		<!-- only one numerical value range or single value allowed -->
		<xsl:choose>
			<xsl:when test="local-name() = 'F_Block_ID'">
				<xsl:choose>
					<xsl:when test="substring-before($ValueList, ' ')">
						<xsl:call-template name="PrintFBlockIDAllowedValueRange">
							<xsl:with-param name="Value" select="substring-before($ValueList, ' ')"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="PrintFBlockIDAllowedValueRange">
							<xsl:with-param name="Value" select="$ValueList"/>
						</xsl:call-template>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:when test="local-name() = 'F_Par_Version'">
				<xsl:if test="$ValueList = 1"><xsl:value-of select="': V2-mode'"/></xsl:if>
			</xsl:when>
			<xsl:when test="local-name() = 'F_WD_Time'">
				<xsl:value-of select="' ms'"/>
			</xsl:when>
		</xsl:choose>
	</xsl:template>


	<!-- Template for F_Block_ID AllowedValues value range-->
	<xsl:template name="PrintFBlockIDAllowedValueRange">
		<xsl:param name="Value"/>
		<xsl:choose>
			<xsl:when test="substring-before($Value, '..')">
				<!-- compact the range to a 2 digit number -->
				<xsl:variable name="Lower" select="substring-before($Value, '..')"/>
				<xsl:variable name="Upper" select="substring-after($Value, '..')"/>
				<xsl:call-template name="PrintFBlockIDAllowedValue">
					<xsl:with-param name="Value" select="concat($Lower mod 4, $Upper mod 4)"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<!-- convert a single value to a range with identical lower and upper value -->
				<xsl:call-template name="PrintFBlockIDAllowedValue">
					<xsl:with-param name="Value" select="concat($Value mod 4, $Value mod 4)"/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- Template for F_Block_ID AllowedValues compact value range -->
	<xsl:template name="PrintFBlockIDAllowedValue">
		<xsl:param name="Value"/>
		<xsl:choose>
			<!-- '00' nothing -->
			<xsl:when test="$Value = '01'"><xsl:value-of select="': supports F_iPar_CRC'"/></xsl:when>
			<xsl:when test="$Value = '02'"><xsl:value-of select="': supports F_iPar_CRC and F_WD_Time_2'"/></xsl:when>
			<xsl:when test="$Value = '03'"><xsl:value-of select="': supports F_iPar_CRC and F_WD_Time_2'"/></xsl:when>
			<xsl:when test="$Value = '11'"><xsl:value-of select="': requires F_iPar_CRC'"/></xsl:when>
			<xsl:when test="$Value = '12'"><xsl:value-of select="': supports F_iPar_CRC and F_WD_Time_2'"/></xsl:when>
			<xsl:when test="$Value = '13'"><xsl:value-of select="': supports F_iPar_CRC and F_WD_Time_2'"/></xsl:when>
			<xsl:when test="$Value = '22'"><xsl:value-of select="': requires F_WD_Time_2'"/></xsl:when>
			<xsl:when test="$Value = '23'"><xsl:value-of select="': supports F_iPar_CRC, requires F_WD_Time_2'"/></xsl:when>
			<xsl:when test="$Value = '33'"><xsl:value-of select="': requires F_iPar_CRC and F_WD_Time_2'"/></xsl:when>
		</xsl:choose>
	</xsl:template>


	<!-- Template for (Profile)ChannelDiagItem representation -->
	<xsl:template name="DisplayChannelDiagItem">
		<tr>
			<td><xsl:value-of select="@ErrorType"/></td>
			<td>-</td>
			<xsl:choose>
				<xsl:when test="@MaintenanceAlarmState">
					<td><xsl:value-of select="@MaintenanceAlarmState"/></td>
				</xsl:when>
				<xsl:otherwise>
					<td style="color:#606060">D</td>
				</xsl:otherwise>
			</xsl:choose>
			<td><xsl:value-of select="@API"/></td>
			<td>
				<xsl:call-template name="GetText">
					<xsl:with-param name="TextId" select="gsdml:Name/@TextId"/>
				</xsl:call-template>
			</td>
			<td>
				<xsl:call-template name="GetText">
					<xsl:with-param name="TextId" select="gsdml:Help/@TextId"/>
				</xsl:call-template>
			</td>
		</tr>
		<xsl:for-each select="gsdml:ExtChannelDiagList/gsdml:ExtChannelDiagItem">
			<xsl:call-template name="DisplayExtChannelDiagItem"/>
		</xsl:for-each>
		<xsl:for-each select="gsdml:ExtChannelDiagList/gsdml:ProfileExtChannelDiagItem">
			<xsl:call-template name="DisplayExtChannelDiagItem"/>
		</xsl:for-each>
	</xsl:template>


	<!-- Template for (Profile)ExtChannelDiagItem representation -->
	<xsl:template name="DisplayExtChannelDiagItem">
		<tr>
			<td><xsl:value-of select="../../@ErrorType"/></td>
			<td><xsl:value-of select="@ErrorType"/></td>
			<xsl:choose>
				<xsl:when test="@MaintenanceAlarmState">
					<td><xsl:value-of select="@MaintenanceAlarmState"/></td>
				</xsl:when>
				<xsl:otherwise>
					<td style="color:#606060">D</td>
				</xsl:otherwise>
			</xsl:choose>
			<td><xsl:value-of select="@API"/></td>
			<td>
				<xsl:call-template name="GetText">
					<xsl:with-param name="TextId" select="gsdml:Name/@TextId"/>
				</xsl:call-template>
			</td>
			<td>
				<xsl:call-template name="GetText">
					<xsl:with-param name="TextId" select="gsdml:Help/@TextId"/>
				</xsl:call-template>
			</td>
		</tr>
		<xsl:for-each select="gsdml:ExtChannelAddValue/gsdml:DataItem">
			<tr>
				<td colspan="4"></td>
				<td colspan="2">AddValue Id=<xsl:value-of select="@Id"/>, <xsl:value-of select="@DataType"/>
					<xsl:if test="@Length">[<xsl:value-of select="@Length"/>]</xsl:if>
				</td>
			</tr>
		</xsl:for-each> <!-- ExtChannelAddValue/DataItem -->
	</xsl:template>


	<xsl:template name="PrintFieldbusType">
		<xsl:param name="Value"/>
		<xsl:choose>
			<xsl:when test="$Value=1">PROFIBUS</xsl:when>
			<xsl:when test="$Value=2">INTERBUS</xsl:when>
			<xsl:when test="$Value=3">AS-Interface</xsl:when>
			<xsl:when test="$Value=4">HART</xsl:when>
			<xsl:when test="$Value=5">DeviceNet</xsl:when>
			<xsl:when test="$Value=6">FF</xsl:when>
			<xsl:when test="$Value=7">CANopen</xsl:when>
			<xsl:otherwise>?</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template name="PrintSubstitutionModes">
		<xsl:param name="ValueList"/>
		<xsl:variable name="Value" select="substring-before($ValueList,' ')"/>
		<xsl:choose>
			<xsl:when test="$Value">
				<xsl:call-template name="PrintSubstitutionMode">
					<xsl:with-param name="Value" select="$Value"/>
				</xsl:call-template>,
				<xsl:call-template name="PrintSubstitutionModes">
					<xsl:with-param name="ValueList" select="substring-after($ValueList,' ')"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="PrintSubstitutionMode">
					<xsl:with-param name="Value" select="$ValueList"/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="PrintSubstitutionMode">
		<xsl:param name="Value"/>
		<xsl:choose>
			<xsl:when test="$Value=0">Zero</xsl:when>
			<xsl:when test="$Value=1">LastValue</xsl:when>
			<xsl:when test="$Value=2">ReplacementValue</xsl:when>
			<xsl:otherwise>?</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template name="PrintMAUTypes">
		<xsl:param name="ValueList"/>
		<xsl:variable name="Value" select="substring-before($ValueList,' ')"/>
		<xsl:choose>
			<xsl:when test="$Value">
				<xsl:call-template name="PrintMAUType">
					<xsl:with-param name="Value" select="$Value"/>
				</xsl:call-template>,
				<xsl:call-template name="PrintMAUTypes">
					<xsl:with-param name="ValueList" select="substring-after($ValueList,' ')"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="PrintMAUType">
					<xsl:with-param name="Value" select="$ValueList"/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="PrintMAUType">
		<xsl:param name="Value"/>
		<xsl:choose>
			<xsl:when test="$Value=5">10BASET</xsl:when>
			<xsl:when test="$Value=10">10BASETXHD</xsl:when>
			<xsl:when test="$Value=11">10BASETXFD</xsl:when>
			<xsl:when test="$Value=12">10BASEFLHD</xsl:when>
			<xsl:when test="$Value=13">10BASEFLFD</xsl:when>
			<xsl:when test="$Value=15">100BASETXHD</xsl:when>
			<xsl:when test="$Value=16">100BASETXFD</xsl:when>
			<xsl:when test="$Value=17">100BASEFXHD</xsl:when>
			<xsl:when test="$Value=18">100BASEFXFD</xsl:when>
			<xsl:when test="$Value=21">1000BASEXHD</xsl:when>
			<xsl:when test="$Value=22">1000BASEXFD</xsl:when>
			<xsl:when test="$Value=23">1000BASELXHD</xsl:when>
			<xsl:when test="$Value=24">1000BASELXFD</xsl:when>
			<xsl:when test="$Value=25">1000BASESXHD</xsl:when>
			<xsl:when test="$Value=26">1000BASESXFD</xsl:when>
			<xsl:when test="$Value=29">1000BASETHD</xsl:when>
			<xsl:when test="$Value=30">1000BASETFD</xsl:when>
			<xsl:when test="$Value=31">10GigBASEFX</xsl:when>
			<xsl:when test="$Value=46">100BASELX10</xsl:when>
			<xsl:when test="$Value=54">100BASEPXFD</xsl:when>
			<xsl:otherwise>?</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template name="PrintFOTypes">
		<xsl:param name="ValueList"/>
		<xsl:variable name="Value" select="substring-before($ValueList,' ')"/>
		<xsl:choose>
			<xsl:when test="$Value">
				<xsl:call-template name="PrintFOType">
					<xsl:with-param name="Value" select="$Value"/>
				</xsl:call-template>,
				<xsl:call-template name="PrintFOTypes">
					<xsl:with-param name="ValueList" select="substring-after($ValueList,' ')"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="PrintFOType">
					<xsl:with-param name="Value" select="$ValueList"/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="PrintFOType">
		<xsl:param name="Value"/>
		<xsl:choose>
			<xsl:when test="$Value=1">9 µm single mode fiber</xsl:when>
			<xsl:when test="$Value=2">50 µm multi mode fiber</xsl:when>
			<xsl:when test="$Value=3">62.5 µm multi mode fiber</xsl:when>
			<xsl:when test="$Value=4">SI-POF NA=0.5</xsl:when>
			<xsl:when test="$Value=5">SI-PCF NA=0.36</xsl:when>
			<xsl:when test="$Value=6">LowNA-POF NA=0.3</xsl:when>
			<xsl:when test="$Value=7">GI-POF</xsl:when>
			<xsl:when test="$Value=8">GI-PCF</xsl:when>
			<xsl:otherwise>?</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template name="FirstAvailableSlot">
		<xsl:param name="ValueList"/>
		<xsl:call-template name="FirstAvailableSlotR">
			<xsl:with-param name="Max" select="0"/>
			<xsl:with-param name="ValueList" select="translate($ValueList, '.', ' ')"/>
		</xsl:call-template>
	</xsl:template>

	<xsl:template name="FirstAvailableSlotR">
		<xsl:param name="Max"/>
		<xsl:param name="ValueList"/>
		<xsl:variable name="FirstVal" select="substring-before($ValueList,' ')"/>
		<xsl:choose>
			<xsl:when test="$FirstVal">
				<xsl:choose>
					<xsl:when test="$Max > $FirstVal">
						<xsl:call-template name="FirstAvailableSlotR">
							<xsl:with-param name="Max" select="$Max"/>
							<xsl:with-param name="ValueList" select="normalize-space(substring-after($ValueList,' '))"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="FirstAvailableSlotR">
							<xsl:with-param name="Max" select="$FirstVal"/>
							<xsl:with-param name="ValueList" select="normalize-space(substring-after($ValueList,' '))"/>
						</xsl:call-template>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="$Max > $ValueList">
						<xsl:value-of select="$Max + 1"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="$ValueList + 1"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template name="PrintBoolean">
		<xsl:param name="Value"/>
		<xsl:param name="BDefault">No</xsl:param>
			<xsl:choose>
				<xsl:when test="$Value='true'">Yes</xsl:when>
				<xsl:when test="$Value='1'">Yes</xsl:when>
				<xsl:when test="$Value='false'">No</xsl:when>
				<xsl:when test="$Value='0'">No</xsl:when>
				<xsl:otherwise>
					<font style="color:#606060"><xsl:value-of select="$BDefault"/></font>
				</xsl:otherwise>
			</xsl:choose>
	</xsl:template>


	<!-- This template prints "number" hexadecimal -->
	<xsl:template name="PrintHex">
		<xsl:param name="number">0</xsl:param>
		<xsl:variable name="low" select="$number mod 16"/>
		<xsl:variable name="high" select="floor($number div 16)"/>
		<xsl:choose>
			<xsl:when test="$high &gt; 0">
				<xsl:call-template name="PrintHex">
					<xsl:with-param name="number" select="$high"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>0x</xsl:text>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:choose>
			<xsl:when test="$low &lt; 10">
				<xsl:value-of select="$low"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:variable name="temp" select="$low - 10"/>
				<xsl:value-of select="translate($temp, '012345', 'ABCDEF')"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- This template prints "number" as one hexadecimal octect without leading '0x' -->
	<xsl:template name="PrintHexOctet">
		<xsl:param name="number">0</xsl:param>
		<xsl:value-of select="substring('0123456789ABCDEF', ($number div 16) + 1, 1)"/>
		<xsl:value-of select="substring('0123456789ABCDEF', ($number mod 16) + 1, 1)"/>
	</xsl:template>


<!--
	<xsl:template name="GetBitmap">
		<xsl:param name="GraphicItemTarget"/>
		<xsl:value-of select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:GraphicsList/gsdml:GraphicItem[@ID=$GraphicItemTarget]/@GraphicFile"/>.bmp
	</xsl:template>
-->

	<!-- This template prints the text with "TextId" from the ExternalTextList according to the "Language" -->
	<xsl:template name="GetText">
		<xsl:param name="TextId"/>
		<xsl:choose>
			<xsl:when test="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ExternalTextList/gsdml:Language[@xml:lang=$Language]/gsdml:Text[@TextId=$TextId]">
				<xsl:value-of select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ExternalTextList/gsdml:Language[@xml:lang=$Language]/gsdml:Text[@TextId=$TextId]/@Value"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ExternalTextList/gsdml:PrimaryLanguage/gsdml:Text[@TextId=$TextId]/@Value"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="GetCategoryName">
		<xsl:param name="CategoryRef"/>
		<xsl:call-template name="GetText">
			<xsl:with-param name="TextId" select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:CategoryList/gsdml:CategoryItem[@ID=$CategoryRef]/@TextId"/>
		</xsl:call-template>
	</xsl:template>

	<xsl:template name="GetCategoryInformation">
		<xsl:param name="CategoryRef"/>
		<xsl:call-template name="GetText">
			<xsl:with-param name="TextId" select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:CategoryList/gsdml:CategoryItem[@ID=$CategoryRef]/InfoText/@TextId"/>
		</xsl:call-template>
	</xsl:template>

	<xsl:template name="GetModuleName">
		<xsl:param name="ModuleItemTarget"/>
		<xsl:call-template name="GetText">
			<xsl:with-param name="TextId" select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ModuleList/gsdml:ModuleItem[@ID=$ModuleItemTarget]/gsdml:ModuleInfo/gsdml:Name/@TextId"/>
		</xsl:call-template>
	</xsl:template>

	<xsl:template name="GetModuleInformation">
		<xsl:param name="ModuleItemTarget"/>
		<xsl:call-template name="GetText">
			<xsl:with-param name="TextId" select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:ModuleList/gsdml:ModuleItem[@ID=$ModuleItemTarget]/gsdml:ModuleInfo/gsdml:InfoText/@TextId"/>
		</xsl:call-template>
	</xsl:template>

	<xsl:template name="GetSubmoduleName">
		<xsl:param name="SubmoduleItemTarget"/>
		<xsl:call-template name="GetText">
			<xsl:with-param name="TextId" select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:SubmoduleList/gsdml:SubmoduleItem[@ID=$SubmoduleItemTarget]/gsdml:ModuleInfo/gsdml:Name/@TextId"/>
		</xsl:call-template>
	</xsl:template>

	<xsl:template name="GetSubmoduleInformation">
		<xsl:param name="SubmoduleItemTarget"/>
		<xsl:call-template name="GetText">
			<xsl:with-param name="TextId" select="/gsdml:ISO15745Profile/gsdml:ProfileBody/gsdml:ApplicationProcess/gsdml:SubmoduleList/gsdml:SubmoduleItem[@ID=$SubmoduleItemTarget]/gsdml:ModuleInfo/gsdml:InfoText/@TextId"/>
		</xsl:call-template>
	</xsl:template>

</xsl:stylesheet>
