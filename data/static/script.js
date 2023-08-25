function add_class() {
    fetch("/static/class_template.html") // Fetch the template file
        .then(response => response.text()) // Get the text content of the template
        .then(htmlContent => {
            const list = document.getElementById("class_list");
            const new_item = document.createElement("li");
            new_item.innerHTML = htmlContent; // Set the content of the new item from the template
            list.appendChild(new_item);
        })
        .catch(error => alert("Failed to fetch class template file: " + error));
}

async function fetch_schedule() {
    const response = await fetch('/schedule');
    const jsonData = await response.json();

    console.info(jsonData);
    
    const list = document.getElementById("class_list");

    jsonData["schedule"].forEach(element => {
        fetch('/static/class_template.html')
            .then(response => response.text())
            .then(template => {
                const class_id = element['name'].replaceAll(' ', '-');
                const rendered = template.replace('{{section_name}}', class_id);

                const new_item = document.createElement("li");
                new_item.innerHTML = rendered;
                list.appendChild(new_item);

                // name
                document.querySelector('#' + class_id + ' #name').value = element['name'];

                // days
                for (const day of ["M", "Tu", "W", "Thu", "F", "Sa", "Su"]) {
                    document.querySelector('#' + class_id + ' #' + day).checked = element[day];
                }

                // times
                for (const time of ["start_time", "end_time"]) {
                    document.querySelector('#' + class_id + ' #' + time).value = element[time];
                }
            })
    });
}

function sync_time() {
    const currentDate = new Date();

    // Get the current year, month, day of the month, hour, and minute
    const currentYear = currentDate.getFullYear();
    const currentMonth = currentDate.getMonth() + 1; // Note: Months are zero-indexed, so we add 1 to get the actual month.
    const currentDay = currentDate.getDate();
    const currentHour = currentDate.getHours();
    const currentMinute = currentDate.getMinutes();

    var form = document.createElement("form");
    var year = document.createElement("input");
    var month = document.createElement("input");
    var day = document.createElement("input");
    var hour = document.createElement("input");
    var minute = document.createElement("input");

    form.method = 'POST';
    form.action = '/time';

    year.value = currentYear;
    year.name = 'year';
    form.appendChild(year);

    month.value = currentMonth;
    month.name = 'month';
    form.appendChild(month);

    day.value = currentDay;
    day.name = 'day';
    form.appendChild(day);

    hour.value = currentHour;
    hour.name = 'hour';
    form.appendChild(hour);

    minute.value = currentMinute;
    minute.name = 'minute';
    form.appendChild(minute);

    document.body.appendChild(form);
    form.submit();
}

fetch_schedule();
