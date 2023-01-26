$(function() {
    $(document).ready(function() { 
       
        /* This stuff corrects errors with the colspan on dual attribute class types ?? */
        $(".heading").each(function() {
            if ($(this).find("td").first().attr("colspan") == 2) {
                var c = 0;
                $('.memberdecls > tbody > tr:not(.heading) > .memItemLeft:not(:last)').each(function() {                    
                    if ( $(this).parent().children().length == 1) {
                        $(this).attr("colspan",2);
                        $(this).css("border-right","1px solid");                        
                        if (c == 0) {
                            $(this).css("border-top-right-radius","7px");  
                        }                        
                    }
                    c=c+1;
                });
            }
        });
        

        //Swap MACROS area
        var hasMR = false; 
        var count = 0;
        var loc = 0;
        $(".memberdecls > tbody > tr.heading > td > h2").each(function() {          
            if ($(this).text() == "\nMacros") {
                hasMR = true;   
                loc=count;
            }
            count = count + 1;
        });


        //also swap navi for macros area
        if (hasMR) {
            var total = $(".memberdecls").length;
            var blockMR = $(".memberdecls").eq(loc);
            
            var blockLast = $(".memberdecls").eq(total-1);
            $(blockMR).remove();
            $(blockMR).insertAfter(blockLast);

            count = 0; loc = 0;
            $(".summary > a").each(function() {          
                if ($(this).text() == "Macros") {
                    loc=count;
                }
                count = count + 1;
            });

            total = $(".summary > a").length;
            blockMR = $(".summary > a").eq(loc);
            
            blockLast = $(".summary > a").eq(total-1);
            $(blockMR).remove();
            $(blockMR).insertAfter(blockLast);
        }

        //Fix Type Functions

        var hasTD = false; 
        count = 0;
        loc = 0;
        $(".memberdecls > tbody > tr.heading > td > h2").each(function() {          
            if ($(this).text() == "\nTypedefs") {
                hasTD = true;   
                loc=count;
            }
            count = count + 1;
        });

        blockMR = $(".memberdecls").eq(loc);

        if (hasTD) {
            var typeFuncsBlock = null;
            $("div.groupHeader").each(function() {
                if ($(this).text() == "Type Functions") {
                    typeFuncsBlock=$(this).parent().parent();
                    $(typeFuncsBlock).remove();
                }
            });
        }

        //This removes an autogen message that screws table headers
        $(".ititle").parent().remove();

        //remove circular border from inner section of cards
        $("div.memproto").css("border-top-left-radius", "0px");
        $("div.memproto").css("border-bottom-left-radius", "0px");
        $("div.memproto").css("border-bottom-right-radius", "0px");

        $("dl.note").css("padding", "8px");

        $(".reflist").addClass("todo");
        $(".reflist").removeClass("reflist");
        
        //May not be necessary
        $(".memberdecls").hide();
        $(".memberdecls").show();

        $("#doc-content").css("height","Calc(100vh - 174px");
        $("#nav-tree").css("height","Calc(100vh - 174px");
        $("#side-nav").css("height","100%");

        $("body").fadeIn(250,function() {

        });
    });
  });