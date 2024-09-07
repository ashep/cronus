import * as React from 'react';
import {route, Link} from 'preact-router';

import CssBaseline from "@mui/material/CssBaseline";
import Typography from "@mui/material/Typography";
import Grid from "@mui/material/Grid";
import Stack from "@mui/material/Stack";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import TextField from "@mui/material/TextField";
import CircularProgress from "@mui/material/CircularProgress";
import WiFiIcon from "@mui/icons-material/Wifi";
import ArrowBackIcon from "@mui/icons-material/ArrowBack";

import * as BTSvc from "../../service/Bluetooth";
import * as WiFiSvc from "../../service/WiFi";

interface Props {
    btConnStatus: BTSvc.ConnStatus
    wifiSvc: WiFiSvc.Service
}

interface State {
    credentials: WiFiSvc.Credentials;
}

export default class WiFi extends React.Component<Props, State> {
    private _connTry: number = 0;

    constructor() {
        super();

        this.state = {
            credentials: {ssid: "", password: ""},
        };
    }

    renderSSIDChooser(): React.JSX.Element {
        return (
            <Box sx={{flexDirection: "column", alignItems: "center"}}>
                <Box sx={{mb: 2}}>
                    Choose your Wi-Fi network:
                </Box>

                {!this.props.wifiSvc.ssidList.length &&
                    <Box sx={{textAlign: "center", mb: 1}}>
                        <CircularProgress/>
                    </Box>
                }

                <Stack direction="column" spacing={1}>
                    {this.props.wifiSvc.ssidList.map(item =>
                        <Button variant={"contained"}
                                startIcon={<WiFiIcon/>}
                                onClick={() => this.setState({
                                    credentials: {
                                        ssid: item,
                                        password: this.state.credentials.password,
                                    }
                                })}
                        >
                            {item}
                        </Button>
                    )}
                    <Button variant={"outlined"} startIcon={<ArrowBackIcon/>} onClick={() => route("/device")}>
                        Back
                    </Button>
                </Stack>
            </Box>
        )
    }

    renderCredentialsForm(): React.JSX.Element {
        return (
            <Box component="form" onSubmit={async e => {
                e.preventDefault();
                this._connTry++;
                await this.props.wifiSvc.requestWiFiConnect(this.state.credentials);
            }}>
                <Stack direction={"column"} spacing={1}>
                    {this.props.wifiSvc.connStatus == WiFiSvc.ConnStatus.DISCONNECTED && this._connTry > 0 &&
                        <Typography variant={"subtitle1"} color={"error"}>
                            Connection failed, check the password
                        </Typography>
                    }
                    <TextField required autoFocus
                               label="Password"
                               variant="outlined"
                               type={"password"}
                               value={this.state.credentials.password}
                               disabled={this.props.wifiSvc.connStatus == WiFiSvc.ConnStatus.CONNECTING}
                               onChange={e => this.setState({
                                   credentials: {
                                       ...this.state.credentials,
                                       password: (e.target as HTMLInputElement).value
                                   }
                               })}
                    />
                    <Button variant={"contained"}
                            type={"submit"}
                            startIcon={this.props.wifiSvc.connStatus == WiFiSvc.ConnStatus.CONNECTING ?
                                <CircularProgress size={20}/> : <WiFiIcon/>}
                            disabled={!this.state.credentials.password.length || this.props.wifiSvc.connStatus == WiFiSvc.ConnStatus.CONNECTING}
                    >
                        {this.props.wifiSvc.connStatus == WiFiSvc.ConnStatus.CONNECTING ? "Connecting" : "Connect"} to {this.state.credentials.ssid}
                    </Button>
                    <Button variant={"outlined"}
                            startIcon={<ArrowBackIcon/>}
                            disabled={this.props.wifiSvc.connStatus == WiFiSvc.ConnStatus.CONNECTING}
                            onClick={() => {
                                this._connTry = 0;
                                this.setState({
                                    credentials: {ssid: "", password: ""},
                                });
                            }}>
                        Other network
                    </Button>
                </Stack>
            </Box>
        )
    }

    renderConnected(): React.JSX.Element {
        return (
            <Stack direction={"column"} spacing={1}>
                {this.props.wifiSvc.connectedSSID &&
                    <Typography variant={"subtitle1"} sx={{mb: 2}}>
                        <WiFiIcon sx={{verticalAlign: "middle"}}/> Connected
                        to <b>{this.props.wifiSvc.connectedSSID}</b>
                    </Typography>
                }
                <Button variant={"contained"}
                        onClick={() => {
                            this._connTry = 0;
                            this.setState({
                                credentials: {ssid: "", password: ""},
                            });
                            this.props.wifiSvc.requestWiFiDisconnect();
                        }}
                        disabled={this.props.wifiSvc.connectedSSID == ""}
                >
                    Disconnect
                </Button>
                <Button variant={"outlined"} startIcon={<ArrowBackIcon/>} onClick={() => route("/device")}>
                    Back
                </Button>
            </Stack>
        )
    }

    render(): React.JSX.Element {
        return <Grid container
                     spacing={0}
                     direction="column"
                     alignItems="center"
                     justifyContent="center"
                     sx={{minHeight: '100vh'}}
        >
            <CssBaseline/>

            {this.props.btConnStatus != BTSvc.ConnStatus.CONNECTED
                ?
                <Box>
                    You're not connected to your Cronus device.
                    Click <Link href={"/device/"}>here</Link> to connect.
                </Box>
                :
                <Stack direction={"column"} spacing={1}>
                    {
                        this.props.wifiSvc.connStatus != WiFiSvc.ConnStatus.CONNECTED &&
                        this.state.credentials.ssid == "" &&
                        this.renderSSIDChooser()
                    }
                    {
                        this.props.wifiSvc.connStatus != WiFiSvc.ConnStatus.CONNECTED &&
                        this.state.credentials.ssid != "" &&
                        this.renderCredentialsForm()
                    }
                    {
                        this.props.wifiSvc.connStatus == WiFiSvc.ConnStatus.CONNECTED &&
                        this.renderConnected()
                    }
                </Stack>
            }
        </Grid>;
    }
}