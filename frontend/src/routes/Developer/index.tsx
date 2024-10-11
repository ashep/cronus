import * as React from "react";
import * as BTSvc from "../../service/Bluetooth";
import * as ConfigSvc from "../../service/Config";

import Grid from "@mui/material/Grid";
import CssBaseline from "@mui/material/CssBaseline";
import Box from "@mui/material/Box";
import {Link, route} from "preact-router";
import Stack from "@mui/material/Stack";
import FormControlLabel from "@mui/material/FormControlLabel";
import Checkbox from "@mui/material/Checkbox";
import Button from "@mui/material/Button";
import ArrowBackIcon from "@mui/icons-material/ArrowBack";
import FormControl from "@mui/material/FormControl";
import InputLabel from "@mui/material/InputLabel";
import Select from "@mui/material/Select";
import MenuItem from "@mui/material/MenuItem";

interface Props {
    btConnStatus: BTSvc.ConnStatus
    cfgSvc: ConfigSvc.Service
}

interface State {
}

export default class Misc extends React.Component<Props, State> {
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
                    Click <Link href={"/device"}>here</Link> to connect.
                </Box>
                :
                <Stack direction={"column"} spacing={3}>
                    <FormControlLabel
                        label="Allow unstable releases"
                        control={
                            <Checkbox checked={this.props.cfgSvc.AllowUnstableFirmware}
                                      onChange={(_, v) => this.props.cfgSvc.SetAllowUnstableFirmware(v)}
                            />
                        }
                    />

                    <FormControl variant="standard">
                        <InputLabel id="ds3231-pin-scl">DS3231 SCL</InputLabel>
                        <Select
                            labelId="ds3231-pin-scl"
                            id="ds3231-pin-scl-select"
                            value={this.props.cfgSvc.RTPPinSCL}
                            label="ds3231-pin-scl"
                            onChange={(_, v) => this.props.cfgSvc.SetRTCPinSCL(v.props.value)}
                        >
                            <MenuItem key={38} value={38}>{38}</MenuItem>
                            <MenuItem key={39} value={39}>{39}</MenuItem>
                        </Select>
                    </FormControl>

                    <FormControl variant="standard">
                        <InputLabel id="ds3231-pin-scl">DS3231 SDA</InputLabel>
                        <Select
                            labelId="ds3231-pin-scl"
                            id="ds3231-pin-scl-select"
                            value={this.props.cfgSvc.RTPPinSDA}
                            label="ds3231-pin-scl"
                            onChange={(_, v) => this.props.cfgSvc.SetRTCPinSDA(v.props.value)}
                        >
                            <MenuItem key={38} value={38}>{38}</MenuItem>
                            <MenuItem key={39} value={39}>{39}</MenuItem>
                        </Select>
                    </FormControl>

                    <Button variant={"outlined"} startIcon={<ArrowBackIcon/>} onClick={() => route("/device/#__dev")}>
                        Back
                    </Button>
                </Stack>
            }
        </Grid>
    }
}