import React, { useState, useEffect } from 'react';
import '../App.css';
import 'bootstrap/dist/css/bootstrap.min.css';

import Button from 'react-bootstrap/Button';
import ButtonGroup from 'react-bootstrap/ButtonGroup';
import { Form } from 'react-bootstrap';

import Highcharts from 'highcharts';
import HighchartsReact from 'highcharts-react-official';
import HighchartsHeatmap from "highcharts/modules/heatmap";

import axios from 'axios';

const backend_path = "http://127.0.0.1:5001";

HighchartsHeatmap(Highcharts);

function Home() {

    interface TOFResponse {
        "ID": number;
        "Address": number;
        "Grid": number[][];
    };

    const [IsLoading, setIsLoading] = useState<boolean>(false);
    const [tofData, setTofData] = useState<TOFResponse>({ID: 0, Address: 0x00, Grid: [[]]});

    useEffect(() => {
        const fetchData = async () => {
            setIsLoading(true);
            try {
                const response = await axios.get(backend_path + "/tof");
                setTofData(response.data);
            } catch (error) {
                ;
            } finally {
                setIsLoading(false);
            }
        };

        // Refresht every 100ms for now
        const intervalId = setInterval(fetchData, 100);

        // Cleanup function to clear the interval on unmount
        return () => clearInterval(intervalId);
    }, []);


    const options = {
        chart: {
            type: 'heatmap',
            marginTop: 40,
            marginBottom: 80,
            plotBorderWidth: 1,
        },


        title: {
            text: 'Sales per employee per weekday',
            style: {
                fontSize: '1em'
            }
        },

        xAxis: {
            categories: ['Alexander', 'Marie', 'Maximilian', 'Sophia', 'Lukas',
                'Maria', 'Leon', 'Anna', 'Tim', 'Laura']
        },

        yAxis: {
            categories: ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday'],
            title: null,
            reversed: true
        },

        accessibility: {
            point: {
                descriptionFormat: '{(add index 1)}. ' +
                    '{series.xAxis.categories.(x)} sales ' +
                    '{series.yAxis.categories.(y)}, {value}.'
            }
        },

        colorAxis: {
            min: 0,
            minColor: '#FFFFFF',
            maxColor: '#0000FF'
        },

        legend: {
            align: 'right',
            layout: 'vertical',
            margin: 0,
            verticalAlign: 'top',
            y: 25,
            symbolHeight: 280
        },

        tooltip: {
            format: '<b>{series.xAxis.categories.(point.x)}</b> sold<br>' +
                '<b>{point.value}</b> items on <br>' +
                '<b>{series.yAxis.categories.(point.y)}</b>'
        },

        series: [{
            name: 'Sales per employee',
            borderWidth: 1,
            data: [[0, 0, 10], [0, 1, 19], [0, 2, 8], [0, 3, 24], [0, 4, 67],
                [1, 0, 92], [1, 1, 58], [1, 2, 78], [1, 3, 117], [1, 4, 48],
                [2, 0, 35], [2, 1, 15], [2, 2, 123], [2, 3, 64], [2, 4, 52],
                [3, 0, 72], [3, 1, 132], [3, 2, 114], [3, 3, 19], [3, 4, 16],
                [4, 0, 38], [4, 1, 5], [4, 2, 8], [4, 3, 117], [4, 4, 115],
                [5, 0, 88], [5, 1, 32], [5, 2, 12], [5, 3, 6], [5, 4, 120],
                [6, 0, 13], [6, 1, 44], [6, 2, 88], [6, 3, 98], [6, 4, 96],
                [7, 0, 31], [7, 1, 1], [7, 2, 82], [7, 3, 32], [7, 4, 30],
                [8, 0, 85], [8, 1, 97], [8, 2, 123], [8, 3, 64], [8, 4, 84],
                [9, 0, 47], [9, 1, 114], [9, 2, 31], [9, 3, 48], [9, 4, 91]],
            dataLabels: {
                enabled: true,
                color: '#000000'
            }
        }],

        responsive: {
            rules: [{
                condition: {
                    maxWidth: 1000
                },
                chartOptions: {
                    yAxis: {
                        labels: {
                            format: '{substr value 0 1}'
                        }
                    }
                }
            }]
        }
    }

    return (
        <div style={{ display: 'flex', justifyContent: 'center' }}>
            <div className="Home">
                <p> TOF ID: {!tofData ? "Loading..." : tofData.ID}, Address: {!tofData ? "Loading..." : tofData.Address}</p>
                <HighchartsReact
                    highcharts={Highcharts}
                    options={options}
                    style="height: 300px; width: 520px; margin: 0 auto"
                />
            </div>
        </div>
    );
}

export default Home;
