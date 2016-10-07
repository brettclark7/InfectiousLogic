function getFiles() {
    console.log("updating list");
    $.get("/ftp", function(data) 
     {
        var $s    = $(".shell ul");
        $s.empty();
        //Look for anchors
        var links = $(data).find("a");
        links.each(function()
        {
            // Log files are formatted YYYY-MM-DD_HHMMSS
            // This will fail in year 3000 :)
            var href = $(this).attr("href");
            if(href.charAt(5) == "2")
            {
                // Make it a bit easier to read
                $s.append("<li><a href=\"" + href + "\">" + $(this).text().substr(0,17) + "</a></li>");
            }
        });
    });
};